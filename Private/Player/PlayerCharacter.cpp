


#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetOnlyOwnerSee(true);
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Attach gun to arms
	GunMesh->AttachToComponent(ArmsMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("SOCKET_Weapon"));

	// Setup player input
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WeaponSway(DeltaTime);
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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnFire);
}

void APlayerCharacter::OnFire()
{
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

void APlayerCharacter::WeaponSway(float DeltaTime)
{
	if (!bIsReloadingOrSwitching)
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
		const FVector2D LookInput = LookActionBinding->GetValue().Get<FVector2D>();

		const FRotator FinalRotation = FRotator(
			LookInput.X * SwayFactor,
			LookInput.Y * SwayFactor,
			LookInput.Y * SwayFactor);

		const FRotator NewRotation = UKismetMathLibrary::RInterpTo(
			ArmsMesh->GetRelativeRotation(),
			FRotator(
				FMath::Clamp(-(InitialRotation.Pitch + FinalRotation.Pitch), MinPitch, MaxPitch),
				FMath::Clamp(
					InitialRotation.Yaw + FinalRotation.Yaw + YawCorrection,
					MinYaw + YawCorrection,
					MaxYaw + YawCorrection),
				FMath::Clamp(InitialRotation.Roll + FinalRotation.Roll, MinRoll, MaxRoll)),
			DeltaTime, InterpSpeed);

		ArmsMesh->SetRelativeRotation(NewRotation);
	}
}

