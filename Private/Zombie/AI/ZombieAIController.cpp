


#include "Zombie/AI/ZombieAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
	}

	GetBlackboardComponent()->SetValueAsEnum(TEXT("MovementState"), uint8(EZombieMovementState::EZMS_Walking));
	GetBlackboardComponent()->SetValueAsVector(TEXT("SpawnLocation"), GetPawn()->GetActorLocation());
}

void AZombieAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
}
