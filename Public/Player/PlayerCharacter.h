

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Weapons/WeaponType.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h" // keep last

UCLASS()
class DIERISE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class USkeletalMeshComponent* ArmsMesh;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float TurnRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float LookUpRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* LookAction;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Recoil)
	UTimelineComponent* RecoilTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Recoil)
	UTimelineComponent* RecoilAnimationTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil)
	UCurveFloat* RecoilCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil)
	UCurveFloat* RecoilAnimationCurve;

	FOnTimelineFloat RecoilTrack;
	FOnTimelineFloat RecoilAnimationTrack;
	FOnTimelineEvent RecoilAnimationFinished;

	UFUNCTION()
	void UpdateRecoil(float RecoilValue);

	UFUNCTION()
	void UpdateRecoilAnimation(float RecoilAnimationValue);

	UFUNCTION()
	void HandleRecoilAnimationFinished();

	void OnFire();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void WeaponSway(float DeltaTime);
	void StartRecoil();
	void StartRecoilAnimation();

private:
	UPROPERTY()
	bool bIsReloadingOrSwitching = false;

	UPROPERTY()
	bool bIsADS = false;

	UPROPERTY()
	bool bIsCrouching = false;

	UPROPERTY()
	EWeaponType CurrentWeaponType;

	UPROPERTY()
	float RecoilVerticalAmount = 0.5f;

	UPROPERTY()
	float RecoilHorizontalAmount = 0.3f;

	UPROPERTY()
	float RecoilDuration = 0.03f;

	UPROPERTY()
	float PreRecoilArmsPitch = 0.f;

	UPROPERTY()
	float PostRecoilArmsPitch = 0.f;

	UPROPERTY()
	float PreRecoilArmsLocationX = 0.f;

	UPROPERTY()
	float PostRecoilArmsLocationX = 0.f;

	UPROPERTY()
	float PullBackAmount = 3.f;

	UPROPERTY()
	bool bHasSetRecoilVariables = false;

	struct FEnhancedInputActionValueBinding* MoveActionBinding;
	struct FEnhancedInputActionValueBinding* LookActionBinding;

	float GetRecoilMultiplier();
	float GetRecoilPitch(float ControlPitch);
	void SetRecoilAnimationVariables();
};
