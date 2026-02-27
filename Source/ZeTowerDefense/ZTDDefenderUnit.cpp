#include "ZTDDefenderUnit.h"
#include "ZTDEnemyUnit.h"
#include "EngineUtils.h"

AZTDDefenderUnit::AZTDDefenderUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	Speed = 0.0f; // Defenders don't move

	// Override collision for defenders - they should be static
	if (CapsuleComponent)
	{
		CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // No physics, just queries
		CapsuleComponent->SetCollisionObjectType(ECC_WorldStatic); // Treat as static object
		CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
		CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		CapsuleComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CapsuleComponent->SetGenerateOverlapEvents(false);
		CapsuleComponent->SetMobility(EComponentMobility::Static); // Static mobility
	}

	// Disable gravity for defenders
	if (RootComponent)
	{
		RootComponent->SetMobility(EComponentMobility::Static);
	}

	// Default to being a placement preview for safety - will be cleared for real units
	bIsPlacementPreview = true;
}

void AZTDDefenderUnit::BeginPlay()
{
	Super::BeginPlay();
	BaseSpeed = Speed;
	BaseFireRate = FireRate;
	BasePower = Power;
	BaseAttackRange = AttackRange;
}

void AZTDDefenderUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Placement previews should not do anything
	if (bIsPlacementPreview)
	{
		return;
	}

	if (bIsDead) return;

	FireCooldownTimer -= DeltaTime;

	// Only find a new target if we don't have one or our current target is dead
	if (!CurrentTarget || !Cast<AZTDUnitBase>(CurrentTarget)->IsAlive())
	{
		FindTarget();
	}

	if (CurrentTarget)
	{
		// Face the target
		FVector Direction = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FaceDirection(Direction, DeltaTime);

		if (FireCooldownTimer <= 0.0f)
		{
			AZTDEnemyUnit* EnemyTarget = Cast<AZTDEnemyUnit>(CurrentTarget);
			if (EnemyTarget && EnemyTarget->IsAlive())
			{
				FireAtTarget(EnemyTarget);
				// Notify enemy it has been attacked so it can divert toward us
				EnemyTarget->NotifyAttackedBy(this);
			}
			FireCooldownTimer = 1.0f / FMath::Max(FireRate, 0.01f);
		}
	}
}

void AZTDDefenderUnit::FindTarget()
{
	AZTDEnemyUnit* ClosestEnemy = FindClosestEnemy();
	CurrentTarget = ClosestEnemy;
}

AZTDEnemyUnit* AZTDDefenderUnit::FindClosestEnemy() const
{
	AZTDEnemyUnit* Closest = nullptr;
	float ClosestDist = AttackRange;

	for (TActorIterator<AZTDEnemyUnit> It(GetWorld()); It; ++It)
	{
		AZTDEnemyUnit* Enemy = *It;
		// Only target enemies this unit type can attack
		if (Enemy && Enemy->IsAlive() && CanAttackUnit(Enemy))
		{
			float Dist = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
			if (Dist < ClosestDist)
			{
				ClosestDist = Dist;
				Closest = Enemy;
			}
		}
	}

	return Closest;
}

int32 AZTDDefenderUnit::Fibonacci(int32 N) const
{
	if (N <= 0) return 1;
	if (N == 1) return 1;

	int32 A = 1, B = 1;
	for (int32 I = 2; I <= N; ++I)
	{
		int32 Temp = A + B;
		A = B;
		B = Temp;
	}
	return B;
}

int32 AZTDDefenderUnit::GetUpgradeCost(int32 CurrentLevel) const
{
	return Fibonacci(CurrentLevel);
}

bool AZTDDefenderUnit::UpgradeSpeed(int32& PlayerPoints)
{
	int32 Cost = GetSpeedUpgradeCost();
	if (PlayerPoints < Cost) return false;

	PlayerPoints -= Cost;
	SpeedLevel++;
	Speed = BaseSpeed + SpeedUpgradeAmount * SpeedLevel;
	return true;
}

bool AZTDDefenderUnit::UpgradeFireRate(int32& PlayerPoints)
{
	int32 Cost = GetFireRateUpgradeCost();
	if (PlayerPoints < Cost) return false;

	PlayerPoints -= Cost;
	FireRateLevel++;
	FireRate = BaseFireRate + FireRateUpgradeAmount * FireRateLevel;
	return true;
}

bool AZTDDefenderUnit::UpgradePower(int32& PlayerPoints)
{
	int32 Cost = GetPowerUpgradeCost();
	if (PlayerPoints < Cost) return false;

	PlayerPoints -= Cost;
	PowerLevel++;
	Power = BasePower + PowerUpgradeAmount * PowerLevel;
	return true;
}

bool AZTDDefenderUnit::UpgradeRange(int32& PlayerPoints)
{
	int32 Cost = GetRangeUpgradeCost();
	if (PlayerPoints < Cost) return false;

	PlayerPoints -= Cost;
	RangeLevel++;
	AttackRange = BaseAttackRange + RangeUpgradeAmount * RangeLevel;
	return true;
}
