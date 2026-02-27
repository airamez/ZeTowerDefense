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
		UE_LOG(LogTemp, Warning, TEXT("Could not find BP_EnemyTank class"));
	}

	static ConstructorHelpers::FClassFinder<AZTDEnemyUnit> HeliClassFinder(TEXT("/Game/Blueprints/BP_EnemyHeli"));
	if (HeliClassFinder.Succeeded())
	{
		HeliClass = HeliClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find BP_EnemyHeli class"));
	}
	EnemiesAlive = 0;
	
	// Wave configuration
	BaseSpeed = 100.0f;
	SpeedIncrement = 10.0f;
	BaseFireRate = 1.0f;
	FireRateIncrement = 0.1f;
	BaseHP = 100.0f;
	HPIncrement = 20.0f;
	BasePower = 10.0f;
	PowerIncrement = 5.0f;
	
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
			UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Found base at %s"), *GameBase->GetActorLocation().ToString());
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
			UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Found Tank class automatically"));
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
			UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Found Heli class automatically"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("WaveSpawner: Could not find Heli class! Please set HeliClass in BP_WaveSpawner"));
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: TankClass=%s, HeliClass=%s"), 
		TankClass ? *TankClass->GetName() : TEXT("NULL"), 
		HeliClass ? *HeliClass->GetName() : TEXT("NULL"));

	// Debug: Check if enemy classes are set
	if (TankClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: TankClass is set to %s"), *TankClass->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WaveSpawner: TankClass is NOT set!"));
	}

	if (HeliClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: HeliClass is set to %s"), *HeliClass->GetName());
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
		UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Wave already in progress!"));
		return;
	}

	CurrentWave++;
	EnemiesKilledThisWave = 0;
	TanksKilledThisWave = 0;
	HelisKilledThisWave = 0;
	bWaveInProgress = true;
	ActiveEnemies.Empty();

	UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Starting wave %d"), CurrentWave);
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

	UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Spawning %d tanks and %d helis for wave %d"), TankCount, HeliCount, CurrentWave);

	// Calculate wave stats
	float WaveSpeed = BaseSpeed + SpeedIncrement * (CurrentWave - 1);
	float WaveFireRate = BaseFireRate + FireRateIncrement * (CurrentWave - 1);
	float WaveHP = BaseHP + HPIncrement * (CurrentWave - 1);
	float WavePower = BasePower + PowerIncrement * (CurrentWave - 1);

	EnemiesAlive = 0;

	// Spawn tanks
	for (int32 i = 0; i < TankCount; ++i)
	{
		if (!TankClass) 
		{
			UE_LOG(LogTemp, Error, TEXT("WaveSpawner: TankClass is null, cannot spawn tank!"));
			continue;
		}

		FVector SpawnLoc = GetSpawnLocation(i, TotalCount, SpawnDistance, TankHeight);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Spawning tank #%d at %s"), i+1, *SpawnLoc.ToString());

		AZTDEnemyUnit* Tank = GetWorld()->SpawnActor<AZTDEnemyUnit>(TankClass, SpawnLoc, TankRotation, SpawnParams);
		if (Tank)
		{
			Tank->InitializeStats(WaveSpeed, WaveFireRate, WaveHP, WavePower, AttackRange);
			Tank->AttackDistanceToBase = AttackDistance;
			Tank->OnUnitDestroyed.AddDynamic(this, &AZTDWaveSpawner::OnEnemyDestroyed);
			ActiveEnemies.Add(Tank);
			EnemiesAlive++;
			UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Tank spawned successfully!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("WaveSpawner: Failed to spawn tank!"));
		}
	}

	// Spawn helis
	for (int32 i = 0; i < HeliCount; ++i)
	{
		if (!HeliClass) 
		{
			UE_LOG(LogTemp, Error, TEXT("WaveSpawner: HeliClass is null, cannot spawn heli!"));
			continue;
		}

		FVector SpawnLoc = GetSpawnLocation(TankCount + i, TotalCount, SpawnDistance, HeliHeight);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Spawning heli #%d at %s"), i+1, *SpawnLoc.ToString());

		AZTDEnemyUnit* Heli = GetWorld()->SpawnActor<AZTDEnemyUnit>(HeliClass, SpawnLoc, HeliRotation, SpawnParams);
		if (Heli)
		{
			Heli->InitializeStats(WaveSpeed, WaveFireRate, WaveHP, WavePower, AttackRange);
			Heli->AttackDistanceToBase = AttackDistance;
			Heli->OnUnitDestroyed.AddDynamic(this, &AZTDWaveSpawner::OnEnemyDestroyed);
			ActiveEnemies.Add(Heli);
			EnemiesAlive++;
			UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Heli spawned successfully!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("WaveSpawner: Failed to spawn heli!"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("WaveSpawner: Finished spawning wave %d. Total enemies: %d"), CurrentWave, EnemiesAlive);
}

FVector AZTDWaveSpawner::GetSpawnLocation(int32 Index, int32 TotalCount, float SpawnDistance, float Height) const
{
	if (!GameBase) return FVector::ZeroVector;

	FVector BaseLocation = GameBase->GetActorLocation();

	// Enemies spawn along a line perpendicular to the base direction
	// Base is at one extreme of the map, enemies come from the opposite side
	// Spread units along the width of the map
	float MapWidth = 4000.0f; // 40 * 100 (Unreal units)
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
	FVector TraceEnd = SpawnPos - FVector(0, 0, 2000.0f); // Trace down

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;

	// Try multiple collision channels for better ground detection
	bool bHitGround = GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams);
	if (!bHitGround)
	{
		bHitGround = GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldDynamic, QueryParams);
	}
	if (!bHitGround)
	{
		bHitGround = GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	}

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
