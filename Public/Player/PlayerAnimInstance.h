

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DIERISE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeBeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly)
	class APlayerCharacter* Character;

	UPROPERTY(BlueprintReadOnly)
	float MoveInputX;

	UPROPERTY(BlueprintReadOnly)
	float MoveInputY;

	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAccelerating;

private:
	bool bIsGamePlaying = false;
};
