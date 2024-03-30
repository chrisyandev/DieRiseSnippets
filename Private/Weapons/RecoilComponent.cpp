


#include "Weapons/RecoilComponent.h"
#include "Player/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TimelineComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"

URecoilComponent::URecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	Controller = Controller == nullptr ? Cast<APlayerController>(Character->Controller) : Controller;

	// Setup recoil timeline
	RecoilTrack.BindDynamic(this, &URecoilComponent::UpdateRecoil);
	Character->RecoilTimeline->AddInterpFloat(RecoilCurve, RecoilTrack);
	Character->RecoilTimeline->SetPlayRate(1.f / RecoilDuration);

	// Setup recoil animation timeline
	RecoilAnimationTrack.BindDynamic(this, &URecoilComponent::UpdateRecoilAnimation);
	Character->RecoilAnimationTimeline->AddInterpFloat(RecoilAnimationCurve, RecoilAnimationTrack);
	RecoilAnimationFinished.BindUFunction(this, FName("HandleRecoilAnimationFinished"));
	Character->RecoilAnimationTimeline->SetTimelineFinishedFunc(RecoilAnimationFinished);
}


void URecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URecoilComponent::UpdateRecoil(float RecoilValue)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FRotator ControlRotation = PlayerController->GetControlRotation();

	float A = GetRecoilPitch(ControlRotation.Pitch);
	float B = A + (RecoilVerticalAmount / GetRecoilMultiplier());
	float NewPitch = FMath::Lerp(A, B, RecoilValue);

	PlayerController->SetControlRotation(
		FRotator(
			NewPitch,
			ControlRotation.Yaw,
			ControlRotation.Roll));

	float YawInput = (FMath::FRand() * 2.f - 1.f) * (RecoilHorizontalAmount / GetRecoilMultiplier());
	Character->AddControllerYawInput(YawInput);
}

void URecoilComponent::UpdateRecoilAnimation(float RecoilAnimationValue)
{
	if (!bHasSetRecoilVariables)
	{
		SetRecoilAnimationVariables();
		bHasSetRecoilVariables = true;
	}

	float NewPitch = FMath::Lerp(PreRecoilArmsPitch, PostRecoilArmsPitch, RecoilAnimationValue);
	Character->ArmsMesh->SetRelativeRotation(
		FRotator(
			NewPitch,
			Character->ArmsMesh->GetRelativeRotation().Yaw,
			Character->ArmsMesh->GetRelativeRotation().Roll));

	float NewLocationX = FMath::Lerp(PreRecoilArmsLocationX, PostRecoilArmsLocationX, RecoilAnimationValue);
	Character->ArmsMesh->SetRelativeLocation(
		FVector(
			NewLocationX,
			Character->ArmsMesh->GetRelativeLocation().Y,
			Character->ArmsMesh->GetRelativeLocation().Z));
}

void URecoilComponent::HandleRecoilAnimationFinished()
{
	bHasSetRecoilVariables = false;
}

void URecoilComponent::StartRecoil()
{
	Character->RecoilTimeline->PlayFromStart();
}

void URecoilComponent::StartRecoilAnimation()
{
	Character->RecoilAnimationTimeline->PlayFromStart();
}

float URecoilComponent::GetRecoilMultiplier()
{
	float A = Character->bHasRecoilReductionPerk ? 1.5f : 0.f;
	float B = Character->bIsCrouched ? 1.5f : 0.f;
	return FMath::Clamp(A + B, 1.0f, 1.8f);
}

float URecoilComponent::GetRecoilPitch(float ControlPitch)
{
	return ControlPitch > 180.f ? ControlPitch - 360.f : ControlPitch;
}

void URecoilComponent::SetRecoilAnimationVariables()
{
	PreRecoilArmsPitch = Character->ArmsMesh->GetRelativeRotation().Pitch;
	PostRecoilArmsPitch = PreRecoilArmsPitch + (RecoilVerticalAmount / GetRecoilMultiplier());

	PreRecoilArmsLocationX = Character->ArmsMesh->GetRelativeLocation().X;
	PostRecoilArmsLocationX = PreRecoilArmsLocationX + (PullBackAmount / GetRecoilMultiplier() * -1.f);
}