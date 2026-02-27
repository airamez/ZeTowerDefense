#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZTDGameOverWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class ZETOWERDEFENSE_API UZTDGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateGameOver(int32 WavesCompleted, int32 TotalKills, int32 FinalPoints);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnExitClicked();

	UTextBlock* GameOverTitle = nullptr;
	UTextBlock* WavesText = nullptr;
	UTextBlock* KillsText = nullptr;
	UTextBlock* PointsText = nullptr;
	UButton* ExitButton = nullptr;
};
