


#include "HealthComponent.h"
#include "DieRise/DebugMacros.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UHealthComponent::GetHealthNormalized() const
{
	return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
}

void UHealthComponent::Damage(AActor* AffectedActor, float Amount, const UDamageType* DamageType, AActor* DamageCauser)
{
	int OldHealth = Health;
	Health = (Health - Amount) < 0 ? 0 : (Health - Amount);

	HandleHealthChanged(AffectedActor, DamageCauser, Amount, OldHealth, Health);

	if (Health == 0)
	{
		HandleOutOfHealth(AffectedActor, DamageCauser, Amount, OldHealth, Health);
	}
}

void UHealthComponent::Heal(AActor* AffectedActor, float Amount, const UDamageType* DamageType, AActor* DamageCauser)
{
	int OldHealth = Health;
	Health = (Health + Amount) > MaxHealth ? MaxHealth : (Health + Amount);

	HandleHealthChanged(AffectedActor, DamageCauser, Amount, OldHealth, Health);
}

void UHealthComponent::SetMaxHealth(AActor* AffectedActor, float Amount, const UDamageType* DamageType, AActor* DamageCauser)
{
	int OldMaxHealth = MaxHealth;
	MaxHealth = Amount;

	HandleMaxHealthChanged(AffectedActor, DamageCauser, Amount, OldMaxHealth, MaxHealth);
}

void UHealthComponent::HandleHealthChanged(AActor* AffectedActor, AActor* DamageCauser, float DesiredAmount, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, AffectedActor);
}

void UHealthComponent::HandleMaxHealthChanged(AActor* AffectedActor, AActor* DamageCauser, float DesiredAmount, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, AffectedActor);
}

void UHealthComponent::HandleOutOfHealth(AActor* AffectedActor, AActor* DamageCauser, float DesiredAmount, float OldValue, float NewValue)
{
	PRINT_SCREEN_F("Dead");
}