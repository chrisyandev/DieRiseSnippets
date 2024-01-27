


#include "Weapons/WeaponComponent.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/WeaponType.h"
#include "Sound/SoundCue.h"
#include "Components/TimelineComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// Setup recoil timeline
	RecoilTrack.BindDynamic(this, &UWeaponComponent::UpdateRecoil);
	Character->RecoilTimeline->AddInterpFloat(RecoilCurve, RecoilTrack);
	Character->RecoilTimeline->SetPlayRate(1.f / RecoilDuration);

	// Setup recoil animation timeline
	RecoilAnimationTrack.BindDynamic(this, &UWeaponComponent::UpdateRecoilAnimation);
	Character->RecoilAnimationTimeline->AddInterpFloat(RecoilAnimationCurve, RecoilAnimationTrack);
	RecoilAnimationFinished.BindUFunction(this, FName("HandleRecoilAnimationFinished"));
	Character->RecoilAnimationTimeline->SetTimelineFinishedFunc(RecoilAnimationFinished);

	CurrentWeaponType = EWeaponType::EWT_AssaultRifle;
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	WeaponSway(DeltaTime);
}

void UWeaponComponent::WeaponSway(float DeltaTime)
{
	if (!Character->bIsReloadingOrSwitching)
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
}

void UWeaponComponent::FireWeapon()
{
	if (Character->bIsReloadingOrSwitching) { return; }

	if (CurrentWeaponType == EWeaponType::EWT_AssaultRifle)
	{
		StartRecoil();
		StartRecoilAnimation();

		UGameplayStatics::SpawnEmitterAttached(
			MuzzleFlash,
			Character->GunMesh,
			"SOCKET_Muzzle",
			FVector(),
			FRotator(0.f, 90.f, 0.f),
			EAttachLocation::SnapToTarget);

		UGameplayStatics::SpawnSoundAttached(
			FireSound,
			Character->GunMesh,
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
				EAttachLocation::KeepWorldPosition);
		}
	}
}

void UWeaponComponent::UpdateRecoil(float RecoilValue)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FRotator ControlRotation = PlayerController->GetControlRotation();

	float A = GetRecoilPitch(ControlRotation.Pitch);
	float B = A + (RecoilVerticalAmount / GetRecoilMultiplier());
	float NewPitch = FMath::Lerp(A, B, RecoilValue);

	PlayerController->SetControlRotation(
		FRotator(
			NewPitch,
			ControlRotation.Yaw,
			ControlRotation.Roll));

	float YawInput = (FMath::FRand() * 2.f - 1.f) * (RecoilHorizontalAmount / GetRecoilMultiplier());
	Character->AddControllerYawInput(YawInput);
}

void UWeaponComponent::UpdateRecoilAnimation(float RecoilAnimationValue)
{
	if (!bHasSetRecoilVariables)
	{
		SetRecoilAnimationVariables();
		bHasSetRecoilVariables = true;
	}

	float NewPitch = FMath::Lerp(PreRecoilArmsPitch, PostRecoilArmsPitch, RecoilAnimationValue);
	Character->ArmsMesh->SetRelativeRotation(
		FRotator(
			NewPitch,
			Character->ArmsMesh->GetRelativeRotation().Yaw,
			Character->ArmsMesh->GetRelativeRotation().Roll));

	float NewLocationX = FMath::Lerp(PreRecoilArmsLocationX, PostRecoilArmsLocationX, RecoilAnimationValue);
	Character->ArmsMesh->SetRelativeLocation(
		FVector(
			NewLocationX,
			Character->ArmsMesh->GetRelativeLocation().Y,
			Character->ArmsMesh->GetRelativeLocation().Z));
}

void UWeaponComponent::HandleRecoilAnimationFinished()
{
	bHasSetRecoilVariables = false;
}

void UWeaponComponent::StartRecoil()
{
	Character->RecoilTimeline->PlayFromStart();
}

void UWeaponComponent::StartRecoilAnimation()
{
	Character->RecoilAnimationTimeline->PlayFromStart();
}

float UWeaponComponent::GetRecoilMultiplier()
{
	float A = Character->bIsADS ? 1.5f : 0.f;
	float B = Character->bIsCrouching ? 1.5f : 0.f;
	return FMath::Clamp(A + B, 1.0f, 1.8f);
}

float UWeaponComponent::GetRecoilPitch(float ControlPitch)
{
	return ControlPitch > 180.f ? ControlPitch - 360.f : ControlPitch;
}

void UWeaponComponent::SetRecoilAnimationVariables()
{
	PreRecoilArmsPitch = Character->ArmsMesh->GetRelativeRotation().Pitch;
	PostRecoilArmsPitch = PreRecoilArmsPitch + (RecoilVerticalAmount / GetRecoilMultiplier());

	PreRecoilArmsLocationX = Character->ArmsMesh->GetRelativeLocation().X;
	PostRecoilArmsLocationX = PreRecoilArmsLocationX + (PullBackAmount / GetRecoilMultiplier() * -1.f);
}
