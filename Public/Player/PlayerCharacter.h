

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
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
	virtual void PostInitializeComponents() override;

public:

	/**
	* General
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* ArmsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWeaponComponent* WeaponComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URecoilComponent* RecoilComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UHealthComponent* HealthComp;

	UPROPERTY(BlueprintReadOnly)
	bool bHasRecoilReductionPerk = false;

	UFUNCTION()
	void OnWeaponFireStart();

	UFUNCTION()
	void OnWeaponFireEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartRecoil();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStopRecoil();

	/**
	* Input
	*/
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

	struct FEnhancedInputActionValueBinding* MoveActionBinding;
	struct FEnhancedInputActionValueBinding* LookActionBinding;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void FirePressed();
	void FireReleased();
	void ReloadPressed();

	/**
	* Weapon Recoil
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Recoil)
	UTimelineComponent* RecoilTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Recoil)
	UTimelineComponent* RecoilAnimationTimeline;

	/**
	* Accessors
	*/
	FORCEINLINE FVector2D GetLookInput() { return LookActionBinding->GetValue().Get<FVector2D>(); }
	FORCEINLINE FVector2D GetMoveInput() { return MoveActionBinding->GetValue().Get<FVector2D>(); }

	/**
	* HUD
	*/
	class APlayerHUD* HUD;

	void UpdateOverlay();
};
