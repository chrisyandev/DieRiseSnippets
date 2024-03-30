

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/WeaponType.h"
#include "Player/PlayerHUD.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeapon_FireStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeapon_FireEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIERISE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/**
	* General
	*/
	UPROPERTY()
	class APlayerCharacter* Character;

	UPROPERTY()
	class APlayerController* Controller;

	UPROPERTY()
	class APlayerHUD* HUD;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWeapon> StartingWeaponClass;

	UPROPERTY(BlueprintReadOnly)
	class AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFiring = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFireOnCooldown = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsReloading = false;

	UPROPERTY(BlueprintReadWrite)
	FName bRecoilType = FName(TEXT("Default"));

	FWeapon_FireStart OnWeaponFireStart;
	FWeapon_FireEnd OnWeaponFireEnd;

	EWeaponType CurrentWeaponType;
	FHUDPackage HUDPackage;

	UFUNCTION(BlueprintCallable)
	void UpdateReloadingAmmo();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	bool CanFire();
	void StartFiring();
	void StopFiring();
	bool CanReload();
	void StartReloading();
	void InterruptReloading();
	void Sway(float DeltaTime);
	void SetHUDCrosshairs(float DeltaTime);

	/**
	* Weapon Effects
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects)
	class USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects)
	class USoundAttenuation* FireSoundAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects)
	class UParticleSystem* BulletImpactParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects)
	class USoundCue* BulletImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects)
	class UMaterialInterface* BulletImpactDecal;
	
	/**
	* Automatic Fire
	*/
	FTimerHandle FireTimer;

	void StartFireTimer();
	void HandleFireTimerFinished();

private:
	void Fire();

};
