


#include "Player/PlayerAnimInstance.h"
#include "Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!bIsGamePlaying) return;

	if (Character == nullptr)
	{
		Character = Cast<APlayerCharacter>(TryGetPawnOwner());
		if (Character == nullptr) return;
	}

	MoveInputX = Character->GetMoveInput().X;
	MoveInputY = Character->GetMoveInput().Y;

	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = Character->GetCharacterMovement()->IsFalling();
	bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	bIsGamePlaying = true;
}
