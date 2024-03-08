

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

UENUM(BlueprintType)
enum class EZombieMovementState : uint8
{
	EZMS_ClimbingOutOfGround UMETA(DisplayName = "Climbing Out Of Ground"),
	EZMS_Walking UMETA(DisplayName = "Walking"),
	EZMS_Running UMETA(DisplayName = "Running")
};

/**
 * 
 */
UCLASS()
class DIERISE_API AZombieAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
	
};
