

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class DIERISE_API UPlayerOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoInMagText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoInReserveText;
};
