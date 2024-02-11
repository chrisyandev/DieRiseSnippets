


#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/WeaponComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerHUD.h"
#include "Player/PlayerOverlayWidget.h"
#include "Components/TextBlock.h"
#include "Weapons/Weapon.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->bUsePawnControlRotation = true;

	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	ArmsMesh->SetupAttachment(FirstPersonCamera);
	ArmsMesh->SetOnlyOwnerSee(true);
	ArmsMesh->bCastDynamicShadow = false;
	ArmsMesh->CastShadow = false;

	WeaponComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComp"));
	RecoilTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RecoilTimeline"));
	RecoilAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RecoilAnimationTimeline"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Setup player input
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

	// Get reference to HUD
	HUD = Cast<APlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateOverlay();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		MoveActionBinding = &EnhancedInputComponent->BindActionValue(MoveAction);
		LookActionBinding = &EnhancedInputComponent->BindActionValue(LookAction);
	}

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::FireReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::ReloadPressed);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WeaponComp->Character = this;
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookAxisVector.Y * LookUpRate);
	AddControllerYawInput(LookAxisVector.X * TurnRate);
}

void APlayerCharacter::FirePressed()
{
	WeaponComp->StartFiring();
}

void APlayerCharacter::FireReleased()
{
	WeaponComp->StopFiring();
}

void APlayerCharacter::ReloadPressed()
{
	WeaponComp->StartReloading();
}

void APlayerCharacter::UpdateOverlay()
{
	HUD->PlayerOverlayWidget->AmmoInMagText->SetText(FText::FromString(FString::Printf(TEXT("%d"), WeaponComp->EquippedWeapon->AmmoInMag)));
	HUD->PlayerOverlayWidget->AmmoInReserveText->SetText(FText::FromString(FString::Printf(TEXT("%d"), WeaponComp->EquippedWeapon->AmmoInReserve)));
}

