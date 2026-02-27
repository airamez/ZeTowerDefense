#pragma once

#include "CoreMinimal.h"
#include "ZTDUIStrings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ZETOWERDEFENSE_API UZTDUIStrings : public UObject
{
	GENERATED_BODY()

public:
	// Wave Pause Screen Strings
	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetWavePauseTitle(int32 WaveNumber);

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetWavePauseEnemiesText(int32 TankCount, int32 HeliCount);

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetWavePauseInstruction();

	// HUD Strings
	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetBaseHPText(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetPointsText(int32 Points);

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetWaveNumberText(int32 WaveNumber);

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetBuildPhaseText();

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetWaveInProgressText();

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetWaveSummaryText();

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetPausedText();

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetGameOverText();

	// Build Menu Strings
	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetBuildTankText(int32 Cost);

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetBuildHeliText(int32 Cost);

	UFUNCTION(BlueprintPure, Category = "UI Strings")
	static FString GetCancelText();

private:
	// String templates
	static const FString WavePauseTitleTemplate;
	static const FString WavePauseEnemiesTemplate;
	static const FString WavePauseInstructionTemplate;
	static const FString BaseHPTemplate;
	static const FString PointsTemplate;
	static const FString WaveNumberTemplate;
	static const FString BuildPhaseTemplate;
	static const FString WaveInProgressTemplate;
	static const FString WaveSummaryTemplate;
	static const FString PausedTemplate;
	static const FString GameOverTemplate;
	static const FString BuildTankTemplate;
	static const FString BuildHeliTemplate;
	static const FString CancelTemplate;
};
