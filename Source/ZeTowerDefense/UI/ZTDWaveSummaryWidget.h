#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZTDWaveSummaryWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class ZETOWERDEFENSE_API UZTDWaveSummaryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateSummary(int32 WaveNumber, int32 EnemiesDestroyed, float BaseHP, int32 Points);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnContinueClicked();

	UTextBlock* TitleText = nullptr;
	UTextBlock* WaveNumberText = nullptr;
	UTextBlock* EnemiesDestroyedText = nullptr;
	UTextBlock* BaseHPText = nullptr;
	UTextBlock* PointsText = nullptr;
	UButton* ContinueButton = nullptr;
};
