#include "ZTDWaveSpawner.h"
#include "ZTDEnemyUnit.h"
#include "ZTDBase.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

AZTDWaveSpawner::AZTDWaveSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Initialize default values
	CurrentWave = 0;
	bWaveInProgress = false;
	EnemiesKilledThisWave = 0;

	// Set enemy classes programmatically
	static ConstructorHelpers::FClassFinder<AZTDEnemyUnit> TankClassFinder(TEXT("/Game/Blueprints/BP_EnemyTank"));
	if (TankClassFinder.Succeeded())
	{
		TankClass = TankClassFinder.Class;
	}
	else
	{
	}

	static ConstructorHelpers::FClassFinder<AZTDEnemyUnit> HeliClassFinder(TEXT("/Game/Blueprints/BP_EnemyHeli"));
	if (HeliClassFinder.Succeeded())
	{
		HeliClass = HeliClassFinder.Class;
	}
	else
	{
	}
	EnemiesAlive = 0;
	
	// Tank stats initialization
	TankBaseSpeed = 150.0f;
	TankSpeedIncrement = 15.0f;
	TankBaseFireRate = 0.3f;
	TankFireRateIncrement = 0.05f;
	TankBaseHP = 100.0f;
	TankHPIncrement = 20.0f;
	TankBasePower = 10.0f;
	TankPowerIncrement = 3.0f;
	TankPointsOnKill = 2;
	
	// Helicopter stats initialization
	HeliBaseSpeed = 250.0f;
	HeliSpeedIncrement = 25.0f;
	HeliBaseFireRate = 0.7f;
	HeliFireRateIncrement = 0.15f;
	HeliBaseHP = 50.0f;
	HeliHPIncrement = 10.0f;
	HeliBasePower = 7.0f;
	HeliPowerIncrement = 2.5f;
	HeliPointsOnKill = 1;
	
	// Spawn configuration
	TankHeight = 100.0f;
	HeliHeight = 300.0f;
}

void AZTDWaveSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Find the base in the world
	TArray<AActor*> FoundBases;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZTDBase::StaticClass(), FoundBases);
	if (FoundBases.Num() > 0)
	{
		GameBase = Cast<AZTDBase>(FoundBases[0]);
		if (GameBase)
		{
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WaveSpawner: No base found in world!"));
	}
	
	// Try to find enemy classes if not set
	if (!TankClass)
	{
		// Try to find a blueprint class named BP_Tank
		UClass* FoundTankClass = LoadObject<UClass>(nullptr, TEXT("/Game/Blueprints/BP_Tank.BP_Tank_C"));
		if (FoundTankClass)
		{
			TankClass = FoundTankClass;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("WaveSpawner: Could not find Tank class! Please set TankClass in BP_WaveSpawner"));
		}
	}
	
	if (!HeliClass)
	{
		// Try to find a blueprint class named BP_Heli
		UClass* FoundHeliClass = LoadObject<UClass>(nullptr, TEXT("/Game/Blueprints/BP_Heli.BP_Heli_C"));
		if (FoundHeliClass)
		{
			HeliClass = FoundHeliClass;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("WaveSpawner: Could not find Heli class! Please set HeliClass in BP_WaveSpawner"));
		}
	}
	

	// Debug: Check if enemy classes are set
	if (TankClass)
	{
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WaveSpawner: TankClass is NOT set!"));
	}

	if (HeliClass)
	{
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WaveSpawner: HeliClass is NOT set!"));
	}
}

void AZTDWaveSpawner::StartNextWave()
{
	if (bWaveInProgress) 
	{
		return;
	}

	CurrentWave++;
	EnemiesKilledThisWave = 0;
	TanksKilledThisWave = 0;
	HelisKilledThisWave = 0;
	bWaveInProgress = true;
	ActiveEnemies.Empty();

	SpawnEnemies();
}

int32 AZTDWaveSpawner::GetTankCountForWave(int32 WaveNumber) const
{
	return InitialTanks + TankIncrement * (WaveNumber - 1);
}

int32 AZTDWaveSpawner::GetHeliCountForWave(int32 WaveNumber) const
{
	return InitialHelis + HeliIncrement * (WaveNumber - 1);
}

float AZTDWaveSpawner::GetSpawnDistanceForWave(int32 WaveNumber) const
{
	float Distance = InitialSpawnDistance + DistanceIncrement * (WaveNumber - 1);
	return FMath::Min(Distance, MaxDistance);
}

void AZTDWaveSpawner::SpawnEnemies()
{
	if (!GameBase) 
	{
		UE_LOG(LogTemp, Error, TEXT("WaveSpawner: Cannot spawn enemies - no base found!"));
		return;
	}

	int32 TankCount = GetTankCountForWave(CurrentWave);
	int32 HeliCount = GetHeliCountForWave(CurrentWave);
	int32 TotalCount = TankCount + HeliCount;
	float SpawnDistance = GetSpawnDistanceForWave(CurrentWave);


	EnemiesAlive = 0;

	// Create array of spawn positions (larger line formation)
	TArray<FVector> SpawnPositions;
	float MapWidth = 8000.0f; // Increased from 4000 to 8000 for larger spread
	float Spacing = MapWidth / FMath::Max(TotalCount, 1);
	
	for (int32 i = 0; i < TotalCount; ++i)
	{
		FVector SpawnLoc = GetSpawnLocation(i, TotalCount, SpawnDistance, 0.0f); // Height will be set per enemy type
		SpawnPositions.Add(SpawnLoc);
	}

	// Shuffle spawn positions for randomness
	for (int32 i = SpawnPositions.Num() - 1; i > 0; --i)
	{
		int32 RandomIndex = FMath::RandRange(0, i);
		SpawnPositions.Swap(i, RandomIndex);
	}

	// Create arrays of enemy types to spawn
	TArray<TSubclassOf<AZTDEnemyUnit>> EnemyClasses;
	TArray<float> EnemyHeights;
	
	// Add tanks
	for (int32 i = 0; i < TankCount; ++i)
	{
		if (TankClass)
		{
			EnemyClasses.Add(TankClass);
			EnemyHeights.Add(TankHeight);
		}
	}
	
	// Add helis
	for (int32 i = 0; i < HeliCount; ++i)
	{
		if (HeliClass)
		{
			EnemyClasses.Add(HeliClass);
			EnemyHeights.Add(HeliHeight);
		}
	}

	// Shuffle enemy types for randomness
	for (int32 i = EnemyClasses.Num() - 1; i > 0; --i)
	{
		int32 RandomIndex = FMath::RandRange(0, i);
		EnemyClasses.Swap(i, RandomIndex);
		EnemyHeights.Swap(i, RandomIndex);
	}

	// Spawn all enemies at random positions
	for (int32 i = 0; i < EnemyClasses.Num(); ++i)
	{
		TSubclassOf<AZTDEnemyUnit> EnemyClass = EnemyClasses[i];
		float EnemyHeight = EnemyHeights[i];
		FVector SpawnLoc = SpawnPositions[i];
		
		// Adjust height for this specific enemy type
		SpawnLoc.Z = EnemyHeight + 500.0f; // Start above ground
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// Determine enemy type and calculate stats
		FString EnemyType;
		float EnemySpeed, EnemyFireRate, EnemyHP, EnemyPower;
		int32 PointsOnKill;
		
		if (EnemyClass == TankClass)
		{
			EnemyType = TEXT("Tank");
			EnemySpeed = TankBaseSpeed + TankSpeedIncrement * (CurrentWave - 1);
			EnemyFireRate = TankBaseFireRate + TankFireRateIncrement * (CurrentWave - 1);
			EnemyHP = TankBaseHP + TankHPIncrement * (CurrentWave - 1);
			EnemyPower = TankBasePower + TankPowerIncrement * (CurrentWave - 1);
			PointsOnKill = TankPointsOnKill;
		}
		else
		{
			EnemyType = TEXT("Heli");
			EnemySpeed = HeliBaseSpeed + HeliSpeedIncrement * (CurrentWave - 1);
			EnemyFireRate = HeliBaseFireRate + HeliFireRateIncrement * (CurrentWave - 1);
			EnemyHP = HeliBaseHP + HeliHPIncrement * (CurrentWave - 1);
			EnemyPower = HeliBasePower + HeliPowerIncrement * (CurrentWave - 1);
			PointsOnKill = HeliPointsOnKill;
		}
		

		AZTDEnemyUnit* Enemy = GetWorld()->SpawnActor<AZTDEnemyUnit>(EnemyClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		if (Enemy)
		{
			Enemy->InitializeStats(EnemySpeed, EnemyFireRate, EnemyHP, EnemyPower, AttackRange);
			Enemy->AttackDistanceToBase = AttackDistance;
			Enemy->PointsOnKill = PointsOnKill; // Set individual points value
			Enemy->OnUnitDestroyed.AddDynamic(this, &AZTDWaveSpawner::OnEnemyDestroyed);
			ActiveEnemies.Add(Enemy);
			EnemiesAlive++;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("WaveSpawner: Failed to spawn %s!"), *EnemyType);
		}
	}

}

FVector AZTDWaveSpawner::GetSpawnLocation(int32 Index, int32 TotalCount, float SpawnDistance, float Height) const
{
	if (!GameBase) return FVector::ZeroVector;

	FVector BaseLocation = GameBase->GetActorLocation();

	// Enemies spawn along a line perpendicular to the base direction
	// Base is at one extreme of the map, enemies come from the opposite side
	// Spread units along the width of the map with larger spacing
	float MapWidth = 8000.0f; // Increased from 4000 to 8000 for larger spread
	float Spacing = MapWidth / FMath::Max(TotalCount, 1);
	float StartY = BaseLocation.Y - MapWidth * 0.5f;
	float YPos = StartY + Spacing * Index + Spacing * 0.5f;

	// Spawn at distance from base along X axis (opposite side)
	float XPos = BaseLocation.X + SpawnDistance;

	// Initial position with height
	FVector SpawnPos = FVector(XPos, YPos, Height + 500.0f); // Start above ground

	// Ground detection
	FHitResult GroundHit;
	FVector TraceStart = SpawnPos;
	FVector TraceEnd = SpawnPos - FVector(0, 0, 1000.0f); // Trace down 1000 units

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	bool bHitGround = GetWorld()->LineTraceSingleByChannel(
		GroundHit,
		TraceStart,
		TraceEnd,
		ECC_WorldStatic,
		QueryParams
	);

	if (bHitGround)
	{
		// Place enemy on ground with proper height
		float CapsuleHalfHeight = 50.0f; // Half of capsule height
		SpawnPos.Z = GroundHit.Location.Z + Height + CapsuleHalfHeight + 10.0f;
	}

	return SpawnPos;
}

void AZTDWaveSpawner::OnEnemyDestroyed(AZTDUnitBase* DestroyedUnit)
{
	AZTDEnemyUnit* Enemy = Cast<AZTDEnemyUnit>(DestroyedUnit);
	if (Enemy)
	{
		ActiveEnemies.Remove(Enemy);
		EnemiesAlive--;
		EnemiesKilledThisWave++;

		// Track kills by unit type
		if (Enemy->UnitType == EZTDUnitType::Tank)
		{
			TanksKilledThisWave++;
		}
		else if (Enemy->UnitType == EZTDUnitType::Heli)
		{
			HelisKilledThisWave++;
		}

		OnEnemyKilled.Broadcast(Enemy);

		if (EnemiesAlive <= 0 && bWaveInProgress)
		{
			bWaveInProgress = false;
			OnWaveComplete.Broadcast(CurrentWave);
		}
	}
}
