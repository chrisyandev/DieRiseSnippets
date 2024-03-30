


#include "Weapons/WeaponComponent.h"
#include "Weapons/RecoilComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/PlayerHUD.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/WeaponType.h"
#include "Weapons/Weapon.h"
#include "Sound/SoundCue.h"
#include "Components/TimelineComponent.h"
#include "Components/DecalComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "TimerManager.h"
#include "DieRise/DebugMacros.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO: Temp way to simulate equipping a weapon
	EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(StartingWeaponClass, Character->GetActorLocation(), Character->GetActorRotation());
	EquippedWeapon->AttachToComponent(Character->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SOCKET_Weapon"));
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Sway(DeltaTime);
	SetHUDCrosshairs(DeltaTime);
}

bool UWeaponComponent::CanFire()
{
	if (bIsFireOnCooldown || bIsReloading || EquippedWeapon->AmmoInMag == 0)
	{
		return false;
	}
	return true;
}

void UWeaponComponent::StartFiring()
{	
	if (CanFire())
	{
		Fire();
		return;
	}

	if (!CanFire() && CanReload())
	{
		StartReloading();
		return;
	}

	if (bIsReloading)
	{
		InterruptReloading();
		if (CanFire())
		{
			Fire();
			return;
		}
	}
}

void UWeaponComponent::StopFiring()
{
	bIsFiring = false;
	OnWeaponFireEnd.Broadcast();
}

bool UWeaponComponent::CanReload()
{
	if (bIsReloading || EquippedWeapon->AmmoInMag == EquippedWeapon->MagCapacity || EquippedWeapon->AmmoInReserve == 0)
	{
		return false;
	}
	return true;
}

void UWeaponComponent::StartReloading()
{
	if (!CanReload()) return;

	StopFiring();

	bIsReloading = true;

	Character->ArmsMesh->GetAnimInstance()->Montage_Play(EquippedWeapon->ReloadMontage);
}

void UWeaponComponent::InterruptReloading()
{
	bIsReloading = false;
	
	Character->ArmsMesh->GetAnimInstance()->Montage_Stop(0.1f, EquippedWeapon->ReloadMontage);
}

void UWeaponComponent::UpdateReloadingAmmo()
{
	int AmmoNeeded = EquippedWeapon->MagCapacity - EquippedWeapon->AmmoInMag;
	int AmmoTakenFromReserve = EquippedWeapon->AmmoInReserve - AmmoNeeded >= 0 ? AmmoNeeded : EquippedWeapon->AmmoInReserve;
	EquippedWeapon->AmmoInMag += AmmoTakenFromReserve;
	EquippedWeapon->AmmoInReserve -= AmmoTakenFromReserve;
}

void UWeaponComponent::FinishReloading()
{
	bIsReloading = false;

	if (bIsFiring && EquippedWeapon->bIsAutomatic)
	{
		StartFiring();
	}
}

void UWeaponComponent::Sway(float DeltaTime)
{
	const float SwayFactor = 1.5f;
	const float YawCorrection = -90.f; // corrects how the original mesh is rotated
	const float MinPitch = -4.f;
	const float MaxPitch = 4.f;
	const float MinYaw = -8.f;
	const float MaxYaw = 8.f;
	const float MinRoll = -2.f;
	const float MaxRoll = 2.f;
	const float InterpSpeed = 3.f;

	const FRotator InitialRotation = FRotator::ZeroRotator;
	const FVector2D LookInput = Character->GetLookInput();

	const FRotator FinalRotation = FRotator(
		LookInput.X * SwayFactor,
		LookInput.Y * SwayFactor,
		LookInput.Y * SwayFactor);

	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(
		Character->ArmsMesh->GetRelativeRotation(),
		FRotator(
			FMath::Clamp(-(InitialRotation.Pitch + FinalRotation.Pitch), MinPitch, MaxPitch),
			FMath::Clamp(
				InitialRotation.Yaw + FinalRotation.Yaw + YawCorrection,
				MinYaw + YawCorrection,
				MaxYaw + YawCorrection),
			FMath::Clamp(InitialRotation.Roll + FinalRotation.Roll, MinRoll, MaxRoll)),
		DeltaTime, InterpSpeed);

	Character->ArmsMesh->SetRelativeRotation(NewRotation);
}

void UWeaponComponent::Fire()
{
	bIsFiring = true;
	StartFireTimer();
	--EquippedWeapon->AmmoInMag;

	if (bRecoilType == FName(TEXT("Default")))
	{
		Character->RecoilComp->StartRecoil();
		Character->RecoilComp->StartRecoilAnimation();
	}
	OnWeaponFireStart.Broadcast();

	UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlash,
		EquippedWeapon->WeaponMesh,
		"SOCKET_Muzzle",
		FVector(),
		FRotator(0.f, 90.f, 0.f),
		EAttachLocation::SnapToTarget);

	UGameplayStatics::SpawnSoundAttached(
		FireSound,
		EquippedWeapon->WeaponMesh,
		FName(),
		FVector(),
		EAttachLocation::SnapToTarget,
		false,
		1.f, 1.f, 0.f,
		FireSoundAttenuation);

	FHitResult OutHit;
	FVector TraceStart = Character->FirstPersonCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (Character->FirstPersonCamera->GetForwardVector() * TRACE_LENGTH);

	GetWorld()->LineTraceSingleByChannel(
		OutHit,
		TraceStart,
		TraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (OutHit.IsValidBlockingHit())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BulletImpactParticle,
			OutHit.ImpactPoint,
			UKismetMathLibrary::MakeRotFromX(OutHit.Normal),
			true);

		UGameplayStatics::SpawnSoundAtLocation(
			GetWorld(),
			BulletImpactSound,
			OutHit.ImpactPoint);

		UGameplayStatics::SpawnDecalAttached(
			BulletImpactDecal,
			FVector(4.f, 4.f, 4.f),
			OutHit.GetComponent(),
			OutHit.BoneName,
			OutHit.ImpactPoint,
			UKismetMathLibrary::MakeRotFromX(OutHit.Normal),
			EAttachLocation::KeepWorldPosition)
			->SetFadeScreenSize(0.001f);

		// Apply Damage
		const float DamageToCause = OutHit.BoneName.ToString() == FString("head") ? EquippedWeapon->Damage : 2 * EquippedWeapon->Damage;
		UGameplayStatics::ApplyDamage(
			OutHit.GetActor(),
			DamageToCause,
			Controller,
			Character,
			UDamageType::StaticClass()
		);
	}
}

void UWeaponComponent::StartFireTimer()
{
	bIsFireOnCooldown = true;

	Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UWeaponComponent::HandleFireTimerFinished, EquippedWeapon->FireDelay);
}

void UWeaponComponent::HandleFireTimerFinished()
{
	bIsFireOnCooldown = false;

	if (bIsFiring && EquippedWeapon->bIsAutomatic)
	{
		StartFiring();
	}
	else
	{
		StopFiring();
	}
}

void UWeaponComponent::SetHUDCrosshairs(float DeltaTime)
{
	Controller = Controller == nullptr ? Cast<APlayerController>(Character->Controller) : Controller;
	
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<APlayerHUD>(Controller->GetHUD()) : HUD;

		if (HUD)
		{
			HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
			HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
			HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
			HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;

			HUDPackage.CrosshairsColor = FLinearColor::White; // TODO: crosshair color

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}
