#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZTDPauseMenuWidget.generated.h"

class UTextBlock;

UCLASS()
class ZETOWERDEFENSE_API UZTDPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
};
