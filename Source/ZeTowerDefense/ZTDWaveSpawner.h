#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZTDWaveSpawner.generated.h"

class AZTDEnemyUnit;
class AZTDBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveComplete, int32, WaveNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyKilled, AZTDEnemyUnit*, KilledEnemy);

UCLASS(Blueprintable)
class ZETOWERDEFENSE_API AZTDWaveSpawner : public AActor
{
	GENERATED_BODY()

public:
	AZTDWaveSpawner();

	// --- Wave Unit Counts ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Units")
	int32 InitialTanks = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Units")
	int32 InitialHelis = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Units")
	int32 TankIncrement = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Units")
	int32 HeliIncrement = 1;

	// --- Tank Stats ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	float TankBaseSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	float TankSpeedIncrement = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	float TankBaseFireRate = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	float TankFireRateIncrement = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	float TankBaseHP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	float TankHPIncrement = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	float TankBasePower = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	float TankPowerIncrement = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Tank")
	int32 TankPointsOnKill = 2;

	// --- Helicopter Stats ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	float HeliBaseSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	float HeliSpeedIncrement = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	float HeliBaseFireRate = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	float HeliFireRateIncrement = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	float HeliBaseHP = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	float HeliHPIncrement = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	float HeliBasePower = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	float HeliPowerIncrement = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Heli")
	int32 HeliPointsOnKill = 1;

	// --- Spawner Distance Parameters ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Distance")
	float InitialSpawnDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Distance")
	float DistanceIncrement = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Distance")
	float MaxDistance = 15000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Distance")
	float AttackDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Distance")
	float AttackRange = 1500.0f;

	// --- Unit Spawn Adjustments ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	float TankHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	float HeliHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	FRotator TankRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Spawn")
	FRotator HeliRotation = FRotator::ZeroRotator;

	// --- Blueprint Classes to Spawn ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Classes")
	TSubclassOf<AZTDEnemyUnit> TankClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Classes")
	TSubclassOf<AZTDEnemyUnit> HeliClass;

	// --- State ---
	UPROPERTY(BlueprintReadOnly, Category = "Wave|State")
	int32 CurrentWave = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Wave|State")
	int32 EnemiesAlive = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Wave|State")
	int32 EnemiesKilledThisWave = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Wave|State")
	int32 TanksKilledThisWave = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Wave|State")
	int32 HelisKilledThisWave = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Wave|State")
	bool bWaveInProgress = false;

	// --- Events ---
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWaveComplete OnWaveComplete;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyKilled OnEnemyKilled;

	// --- Functions ---
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartNextWave();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 GetTankCountForWave(int32 WaveNumber) const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 GetHeliCountForWave(int32 WaveNumber) const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	float GetSpawnDistanceForWave(int32 WaveNumber) const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 GetTanksKilledThisWave() const { return TanksKilledThisWave; }

	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 GetHelisKilledThisWave() const { return HelisKilledThisWave; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnEnemyDestroyed(AZTDUnitBase* DestroyedUnit);

	void SpawnEnemies();
	FVector GetSpawnLocation(int32 Index, int32 TotalCount, float SpawnDistance, float Height) const;

	UPROPERTY()
	AZTDBase* GameBase = nullptr;

	UPROPERTY()
	TArray<AZTDEnemyUnit*> ActiveEnemies;
};
