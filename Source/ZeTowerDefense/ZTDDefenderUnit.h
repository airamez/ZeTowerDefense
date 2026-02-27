#pragma once

#include "CoreMinimal.h"
#include "ZTDUnitBase.h"
#include "ZTDDefenderUnit.generated.h"

class AZTDEnemyUnit;

UCLASS(Blueprintable)
class ZETOWERDEFENSE_API AZTDDefenderUnit : public AZTDUnitBase
{
	GENERATED_BODY()

public:
	AZTDDefenderUnit();

	virtual void Tick(float DeltaTime) override;

	// Upgrade levels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	int32 SpeedLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	int32 FireRateLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	int32 PowerLevel = 0;

	// Base stats before upgrades
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BaseSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BaseFireRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BasePower = 10.0f;

	// Upgrade increment per level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	float SpeedUpgradeAmount = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	float FireRateUpgradeAmount = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	float PowerUpgradeAmount = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	float RangeUpgradeAmount = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	int32 RangeLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BaseAttackRange = 1500.0f;

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	int32 GetUpgradeCost(int32 CurrentLevel) const;

	// Flag to identify placement previews (not real units)
	UPROPERTY(BlueprintReadOnly, Category = "Unit")
	bool bIsPlacementPreview = false;

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	bool UpgradeSpeed(int32& PlayerPoints);

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	bool UpgradeFireRate(int32& PlayerPoints);

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	bool UpgradePower(int32& PlayerPoints);

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	bool UpgradeRange(int32& PlayerPoints);

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	int32 GetSpeedUpgradeCost() const { return GetUpgradeCost(SpeedLevel); }

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	int32 GetFireRateUpgradeCost() const { return GetUpgradeCost(FireRateLevel); }

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	int32 GetPowerUpgradeCost() const { return GetUpgradeCost(PowerLevel); }

	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	int32 GetRangeUpgradeCost() const { return GetUpgradeCost(RangeLevel); }

protected:
	virtual void BeginPlay() override;
	virtual void FindTarget() override;

	AZTDEnemyUnit* FindClosestEnemy() const;

	int32 Fibonacci(int32 N) const;
};
