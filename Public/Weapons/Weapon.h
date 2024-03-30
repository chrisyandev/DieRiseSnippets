

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class DIERISE_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MagazineMesh;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsBottom;

	/**
	* Weapon Stats
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float FireDelay = .1f;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	bool bIsAutomatic = true;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int MagCapacity = 30;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int AmmoInMag = MagCapacity;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int AmmoInReserve = 90;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int Damage = 10;

	/**
	* Weapon Animations
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Animations")
	class UAnimMontage* ReloadMontage;
};
