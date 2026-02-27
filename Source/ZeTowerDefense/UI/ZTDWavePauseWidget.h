#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZTDWavePauseWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS(Blueprintable)
class ZETOWERDEFENSE_API UZTDWavePauseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WavePause")
	void UpdateWaveInfo(int32 WaveNumber, int32 TankCount, int32 HeliCount);

	UFUNCTION(BlueprintCallable, Category = "WavePause")
	void ShowAsPaused();

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "WavePause")
	UTextBlock* TitleText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "WavePause")
	UTextBlock* WaveNumberText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "WavePause")
	UTextBlock* TankCountText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "WavePause")
	UTextBlock* HeliCountText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "WavePause")
	UTextBlock* StartHintText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "WavePause")
	UButton* StartButton = nullptr;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnStartClicked();

	UPROPERTY()
	int32 CurrentWaveNumber = 0;
};
