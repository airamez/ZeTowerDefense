#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZTDGameMode.generated.h"

class AZTDBase;
class AZTDWaveSpawner;
class AZTDEnemyUnit;
class AZTDUnitBase;

UENUM(BlueprintType)
enum class EZTDGameState : uint8
{
	BuildPhase,
	WaveInProgress,
	WaveSummary,
	Paused,
	GameOver
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointsChanged, int32, NewPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EZTDGameState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveNumberChanged, int32, WaveNumber);

UCLASS(Blueprintable)
class ZETOWERDEFENSE_API AZTDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AZTDGameMode();

	// --- Points ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 PlayerPoints = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 PointsPerKill = 1;

	// --- State ---
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	EZTDGameState CurrentGameState = EZTDGameState::BuildPhase;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	int32 CurrentWaveNumber = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	int32 TotalEnemiesKilled = 0;

	// --- Events ---
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPointsChanged OnPointsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameStateChanged OnGameStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWaveNumberChanged OnWaveNumberChanged;

	// --- References ---
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	AZTDBase* GameBase = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	AZTDWaveSpawner* WaveSpawner = nullptr;

	// --- Classes to Spawn ---
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	TSubclassOf<AZTDBase> BaseClass;

	// --- Functions ---
	UFUNCTION(BlueprintCallable, Category = "Game")
	void AddPoints(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Game")
	bool SpendPoints(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartNextWave();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ActuallyStartWave();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ExitGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void DismissWaveSummary();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetGameState(EZTDGameState NewState);

	UFUNCTION(BlueprintCallable, Category = "Game")
	int32 GetPlayerPoints() const { return PlayerPoints; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	float GetBaseHP() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnWaveComplete(int32 WaveNumber);

	UFUNCTION()
	void OnEnemyKilled(AZTDEnemyUnit* KilledEnemy);

	UFUNCTION()
	void OnBaseDestroyed();

	EZTDGameState StateBeforePause = EZTDGameState::BuildPhase;
};
