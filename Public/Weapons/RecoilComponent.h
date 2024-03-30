

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "RecoilComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class DIERISE_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URecoilComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/**
	* General
	*/
	UPROPERTY()
	class APlayerCharacter* Character;

	UPROPERTY()
	class APlayerController* Controller;

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
};
