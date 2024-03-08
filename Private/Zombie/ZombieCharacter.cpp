


#include "Zombie/ZombieCharacter.h"
#include "HealthComponent.h"

AZombieCharacter::AZombieCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
}

void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	OnTakeAnyDamage.AddDynamic(this, &AZombieCharacter::ReceiveDamage);
}

void AZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZombieCharacter::ReceiveDamage(AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (DamageAmount > 0)
	{
		HealthComp->Damage(DamagedActor, DamageAmount, DamageType, DamageCauser);
	}
	else if (DamageAmount < 0)
	{
		HealthComp->Heal(DamagedActor, DamageAmount, DamageType, DamageCauser);
	}
}

