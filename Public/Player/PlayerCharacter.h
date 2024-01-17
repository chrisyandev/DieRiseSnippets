

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
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

	void OnFire();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void WeaponSway(float DeltaTime);

private:
	UPROPERTY()
	bool bIsReloadingOrSwitching;

	UPROPERTY()
	bool bIsADS;

	struct FEnhancedInputActionValueBinding* MoveActionBinding;
	struct FEnhancedInputActionValueBinding* LookActionBinding;
};
