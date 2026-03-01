#include "CoreMinimal.h"
#include "ZTDGameMode.h"
#include "ZTDBase.h"
#include "ZTDWaveSpawner.h"
#include "ZTDEnemyUnit.h"
#include "ZTDPlayerController.h"
#include "ZTDHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

AZTDGameMode::AZTDGameMode()
{
	PlayerControllerClass = AZTDPlayerController::StaticClass();
	HUDClass = AZTDHUD::StaticClass();

	// Set Base class programmatically
	static ConstructorHelpers::FClassFinder<AZTDBase> BaseClassFinder(TEXT("/Game/Blueprints/BP_Base"));
	if (BaseClassFinder.Succeeded())
	{
		BaseClass = BaseClassFinder.Class;
	}
	else
	{
			}
}

void AZTDGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Find base
	TArray<AActor*> FoundBases;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZTDBase::StaticClass(), FoundBases);
	if (FoundBases.Num() > 0)
	{
		GameBase = Cast<AZTDBase>(FoundBases[0]);
		if (GameBase)
		{
			GameBase->OnBaseDestroyed.AddDynamic(this, &AZTDGameMode::OnBaseDestroyed);
		}
	}
	else
	{
		// Spawn a base if none exists in the level
		if (BaseClass)
		{
			FVector SpawnLocation = FVector(0, 0, 100); // Center of map, slightly above ground
			FRotator SpawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			GameBase = GetWorld()->SpawnActor<AZTDBase>(BaseClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (GameBase)
			{
				GameBase->OnBaseDestroyed.AddDynamic(this, &AZTDGameMode::OnBaseDestroyed);
			}
		}
	}

	// Find wave spawner
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZTDWaveSpawner::StaticClass(), FoundSpawners);
	if (FoundSpawners.Num() > 0)
	{
		WaveSpawner = Cast<AZTDWaveSpawner>(FoundSpawners[0]);
		if (WaveSpawner)
		{
			WaveSpawner->OnWaveComplete.AddDynamic(this, &AZTDGameMode::OnWaveComplete);
			WaveSpawner->OnEnemyKilled.AddDynamic(this, &AZTDGameMode::OnEnemyKilled);
		}
	}

	SetGameState(EZTDGameState::BuildPhase);
	OnPointsChanged.Broadcast(PlayerPoints);

	// No need to call ShowWavePause - the HUD will draw the screen automatically based on game state
}

void AZTDGameMode::AddPoints(int32 Amount)
{
	PlayerPoints += Amount;
	OnPointsChanged.Broadcast(PlayerPoints);
}

bool AZTDGameMode::SpendPoints(int32 Amount)
{
	if (PlayerPoints < Amount) return false;
	PlayerPoints -= Amount;
	OnPointsChanged.Broadcast(PlayerPoints);
	return true;
}

void AZTDGameMode::StartNextWave()
{
	if (CurrentGameState != EZTDGameState::BuildPhase) return;

	// Just set the state - HUD will handle showing the screen
	SetGameState(EZTDGameState::WaveSummary);
}

void AZTDGameMode::ActuallyStartWave()
{
	if (!WaveSpawner) return;

	// Actually start the wave
	WaveSpawner->StartNextWave();
	CurrentWaveNumber = WaveSpawner->CurrentWave;
	SetGameState(EZTDGameState::WaveInProgress);
	OnWaveNumberChanged.Broadcast(CurrentWaveNumber);
}

void AZTDGameMode::PauseGame()
{
	if (CurrentGameState == EZTDGameState::GameOver) return;
	if (CurrentGameState == EZTDGameState::Paused) return;

	StateBeforePause = CurrentGameState;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	SetGameState(EZTDGameState::Paused);
}

void AZTDGameMode::ResumeGame()
{
	if (CurrentGameState != EZTDGameState::Paused) return;

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	SetGameState(StateBeforePause);
}

void AZTDGameMode::ExitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void AZTDGameMode::DismissWaveSummary()
{
	if (CurrentGameState != EZTDGameState::WaveSummary) return;

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	SetGameState(EZTDGameState::BuildPhase);
}

void AZTDGameMode::SetGameState(EZTDGameState NewState)
{
	CurrentGameState = NewState;
	OnGameStateChanged.Broadcast(NewState);
}

float AZTDGameMode::GetBaseHP() const
{
	return GameBase ? GameBase->CurrentHP : 0.0f;
}

void AZTDGameMode::OnWaveComplete(int32 WaveNumber)
{
	// Don't pause and don't show summary - go directly to build phase
	SetGameState(EZTDGameState::BuildPhase);
}

void AZTDGameMode::OnEnemyKilled(AZTDEnemyUnit* KilledEnemy)
{
	TotalEnemiesKilled++;
	if (KilledEnemy)
	{
		AddPoints(KilledEnemy->PointsOnKill);
	}
	else
	{
		AddPoints(PointsPerKill); // Fallback to global value
	}
}

void AZTDGameMode::OnBaseDestroyed()
{
	SetGameState(EZTDGameState::GameOver);
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Show game over on player controller
	AZTDPlayerController* PC = Cast<AZTDPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->ShowGameOver(CurrentWaveNumber, TotalEnemiesKilled, PlayerPoints);
	}
}
