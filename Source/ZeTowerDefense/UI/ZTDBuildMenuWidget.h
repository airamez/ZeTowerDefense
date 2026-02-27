#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZTDBuildMenuWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class ZETOWERDEFENSE_API UZTDBuildMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnBuildTankClicked();

	UFUNCTION()
	void OnBuildHeliClicked();

	UFUNCTION()
	void OnCancelClicked();

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "BuildMenu")
	UButton* BuildTankButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "BuildMenu")
	UButton* BuildHeliButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "BuildMenu")
	UButton* CancelButton = nullptr;

	UTextBlock* TankCostText = nullptr;
	UTextBlock* HeliCostText = nullptr;
};
