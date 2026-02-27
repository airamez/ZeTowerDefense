#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZTDUnitMenuWidget.generated.h"

class UButton;
class UTextBlock;
class AZTDDefenderUnit;

UCLASS()
class ZETOWERDEFENSE_API UZTDUnitMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetUnit(AZTDDefenderUnit* Unit);
	void RefreshStats();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnUpgradeSpeedClicked();

	UFUNCTION()
	void OnUpgradeFireRateClicked();

	UFUNCTION()
	void OnUpgradePowerClicked();

	UFUNCTION()
	void OnUpgradeHPClicked();

	UFUNCTION()
	void OnCloseClicked();

	UTextBlock* UnitNameText = nullptr;
	UTextBlock* SpeedText = nullptr;
	UTextBlock* FireRateText = nullptr;
	UTextBlock* PowerText = nullptr;
	UTextBlock* HPText = nullptr;
	UTextBlock* SpeedCostText = nullptr;
	UTextBlock* FireRateCostText = nullptr;
	UTextBlock* PowerCostText = nullptr;
	UTextBlock* HPCostText = nullptr;
	UButton* UpgradeSpeedButton = nullptr;
	UButton* UpgradeFireRateButton = nullptr;
	UButton* UpgradePowerButton = nullptr;
	UButton* UpgradeHPButton = nullptr;
	UButton* CloseButton = nullptr;

	UPROPERTY()
	AZTDDefenderUnit* TargetUnit = nullptr;
};
