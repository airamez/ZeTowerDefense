#pragma once

#include "CoreMinimal.h"
#include "ZTDUnitBase.h"
#include "ZTDEnemyUnit.generated.h"

class AZTDBase;
class AZTDDefenderUnit;
class UMovementComponent;

UCLASS(Blueprintable)
class ZETOWERDEFENSE_API AZTDEnemyUnit : public AZTDUnitBase
{
	GENERATED_BODY()

public:
	AZTDEnemyUnit();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamage = 1.0f;

	UPROPERTY()
	AZTDBase* TargetBase = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDistanceToBase = 2000.0f;

	// Set when a defender fires at this enemy - divert to attack that defender
	UPROPERTY()
	AZTDDefenderUnit* AggroTarget = nullptr;

	// Points awarded when this enemy is destroyed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 PointsOnKill = 1;

	// Called by a defender when it fires at this enemy
	void NotifyAttackedBy(AZTDDefenderUnit* Attacker);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMovementComponent* MovementComponent = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeStats(float InSpeed, float InFireRate, float InHP, float InPower, float InAttackRange);

protected:
	virtual void BeginPlay() override;
	virtual void FindTarget() override;

	void MoveTowardBase(float DeltaTime);
	AZTDDefenderUnit* FindClosestDefender() const;

	bool bHasReachedAttackPosition = false;

	// Move toward aggro'd defender instead of base
	void MoveTowardTarget(AActor* Target, float DeltaTime);
};
