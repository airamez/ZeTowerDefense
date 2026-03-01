#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZTDPlayerController.generated.h"

class AZTDDefenderUnit;
class AZTDGameMode;
class UZTDBuildMenuWidget;
class UZTDUnitMenuWidget;
class UZTDPauseMenuWidget;
class UZTDWaveSummaryWidget;
class UZTDGameOverWidget;
class UZTDWavePauseWidget;
class UZTDInstructionsWidget;

UENUM(BlueprintType)
enum class EZTDBuildType : uint8
{
	None,
	Tank,
	Heli
};

UCLASS(Blueprintable)
class ZETOWERDEFENSE_API AZTDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AZTDPlayerController();

	// Widget classes to spawn (set in Blueprint or defaults)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UZTDUnitMenuWidget> UnitMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UZTDPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UZTDWaveSummaryWidget> WaveSummaryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UZTDWavePauseWidget> WavePauseWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UZTDGameOverWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> BuildMenuBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UZTDBuildMenuWidget> BuildMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UZTDInstructionsWidget> InstructionsWidgetClass;

	// Defender classes to build
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TSubclassOf<AZTDDefenderUnit> DefenderTankClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TSubclassOf<AZTDDefenderUnit> DefenderHeliClass;

	// Placement preview
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	UMaterialInterface* ValidPlacementMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	UMaterialInterface* InvalidPlacementMaterial;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowBuildMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideBuildMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SelectBuildType(EZTDBuildType Type);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowUnitMenu(AZTDDefenderUnit* Unit);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideUnitMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowWaveSummary(int32 WaveNumber, int32 EnemiesDestroyed, float BaseHP, int32 Points);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideWaveSummary();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameOver(int32 WavesCompleted, int32 TotalKills, int32 FinalPoints);

	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameOver();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInstructions();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideInstructions();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void CancelBuilding();

	UFUNCTION(BlueprintCallable, Category = "Game")
	AZTDGameMode* GetZTDGameMode() const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	void HandleBuildMenu();
	void HandleRightClick();
	void HandleLeftClick();
	void HandleEscape();
	void HandleContinue();
	void HandleExit();
	void HandleSpace();

	bool TryPlaceUnit();
	bool IsValidPlacement(const FVector& Location) const;
	FVector GetPlacementLocation() const;

	UPROPERTY()
	UUserWidget* BuildMenuWidget = nullptr;

	UPROPERTY()
	UZTDUnitMenuWidget* UnitMenuWidget = nullptr;

	UPROPERTY()
	UZTDPauseMenuWidget* PauseMenuWidget = nullptr;

	UPROPERTY()
	UZTDWaveSummaryWidget* WaveSummaryWidget = nullptr;

	UPROPERTY()
	UZTDGameOverWidget* GameOverWidget = nullptr;

	UPROPERTY()
	UZTDWavePauseWidget* WavePauseWidget = nullptr;

	UPROPERTY()
	UZTDInstructionsWidget* InstructionsWidget = nullptr;

	UPROPERTY()
	AZTDDefenderUnit* SelectedDefender = nullptr;

	UPROPERTY()
	AActor* PlacementPreview = nullptr;

	// Timer for forcing build menu visibility
	FTimerHandle BuildMenuVisibilityTimer;

	EZTDBuildType CurrentBuildType = EZTDBuildType::None;
	bool bIsBuildMenuOpen = false;
	bool bIsPlacing = false;
	bool bIsPaused = false;
	bool bIsWaveSummaryOpen = false;
	bool bSkipInstructionsOnNextBuildPhase = false;
};
