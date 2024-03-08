

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHealth_AttributeChanged, UHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, OwningActor);

UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIERISE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category = Health)
	int Health = 100;

	UPROPERTY(EditAnywhere, Category = Health)
	int MaxHealth = 100;

	UPROPERTY(VisibleAnywhere, Category = Health)
	EDeathState DeathState = EDeathState::NotDead;

	UFUNCTION(BlueprintPure, Category = Health)
	static UHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHealthComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = Health)
	void Damage(AActor* AffectedActor, float Amount, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = Health)
	void Heal(AActor* AffectedActor, float Amount, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = Health)
	void SetMaxHealth(AActor* AffectedActor, float Amount, const UDamageType* DamageType, AActor* DamageCauser);

public:
	UPROPERTY(BlueprintAssignable)
	FHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FHealth_AttributeChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FHealth_DeathEvent OnDeathStarted;

	UPROPERTY(BlueprintAssignable)
	FHealth_DeathEvent OnDeathFinished;

	virtual void HandleHealthChanged(AActor* AffectedActor, AActor* DamageCauser, float DesiredAmount, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* AffectedActor, AActor* DamageCauser, float DesiredAmount, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* AffectedActor, AActor* DamageCauser, float DesiredAmount, float OldValue, float NewValue);
};
