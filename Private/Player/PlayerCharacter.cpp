


#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/WeaponComponent.h"
#include "Components/TimelineComponent.h"

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

	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	RecoilTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RecoilTimeline"));
	RecoilAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RecoilAnimationTimeline"));
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

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Weapon->Character = this;
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

void APlayerCharacter::OnFire()
{
	Weapon->FireWeapon();
}

