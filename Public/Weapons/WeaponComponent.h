

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/WeaponType.h"
#include "Player/PlayerHUD.h"
#include "Components/TimelineComponent.h"
#include "WeaponComponent.generated.h"

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

	UPROPERTY()
	class AWeapon* EquippedWeapon;

	EWeaponType CurrentWeaponType;
	FHUDPackage HUDPackage;

	bool bIsFirePressed = false;
	bool bCanFire = true;

	void StartFireWeapon();
	void StopFireWeapon();
	void StartReloadWeapon();
	void StopReloadWeapon();
	void WeaponSway(float DeltaTime);
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
	* Weapon Recoil
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil)
	UCurveFloat* RecoilCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil)
	UCurveFloat* RecoilAnimationCurve;

	FOnTimelineFloat RecoilTrack;
	FOnTimelineFloat RecoilAnimationTrack;
	FOnTimelineEvent RecoilAnimationFinished;

	float RecoilVerticalAmount = 0.5f;
	float RecoilHorizontalAmount = 0.3f;
	float RecoilDuration = 0.03f;
	float PreRecoilArmsPitch = 0.f;
	float PostRecoilArmsPitch = 0.f;
	float PreRecoilArmsLocationX = 0.f;
	float PostRecoilArmsLocationX = 0.f;
	float PullBackAmount = 3.f;
	bool bHasSetRecoilVariables = false;

	UFUNCTION()
	void UpdateRecoil(float RecoilValue);

	UFUNCTION()
	void UpdateRecoilAnimation(float RecoilAnimationValue);

	UFUNCTION()
	void HandleRecoilAnimationFinished();

	void StartRecoil();
	void StartRecoilAnimation();
	float GetRecoilMultiplier();
	float GetRecoilPitch(float ControlPitch);
	void SetRecoilAnimationVariables();
	
	/**
	* Automatic Fire
	*/
	FTimerHandle FireTimer;

	void StartFireTimer();
	void HandleFireTimerFinished();

private:
	void FireWeapon();

};
