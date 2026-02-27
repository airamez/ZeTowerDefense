#include "ZTDPlayerController.h"
#include "ZTDGameMode.h"
#include "ZTDDefenderUnit.h"
#include "ZTDBase.h"
#include "ZTDWaveSpawner.h"
#include "UI/ZTDBuildMenuWidget.h"
#include "UI/ZTDUnitMenuWidget.h"
#include "UI/ZTDPauseMenuWidget.h"
#include "UI/ZTDWaveSummaryWidget.h"
#include "UI/ZTDGameOverWidget.h"
#include "UI/ZTDWavePauseWidget.h"
#include "UI/ZTDInstructionsWidget.h"
#include "UI/ZTDUIStrings.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

AZTDPlayerController::AZTDPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// Set default widget classes (these will be created programmatically)
	BuildMenuWidgetClass = UZTDBuildMenuWidget::StaticClass();
	UnitMenuWidgetClass = UZTDUnitMenuWidget::StaticClass();
	PauseMenuWidgetClass = UZTDPauseMenuWidget::StaticClass();
	WaveSummaryWidgetClass = UZTDWaveSummaryWidget::StaticClass();
	GameOverWidgetClass = UZTDGameOverWidget::StaticClass();
	WavePauseWidgetClass = UZTDWavePauseWidget::StaticClass();
	InstructionsWidgetClass = UZTDInstructionsWidget::StaticClass();

	// Set defender classes programmatically
	static ConstructorHelpers::FClassFinder<AZTDDefenderUnit> TankClassFinder(TEXT("/Game/Blueprints/BP_DefenderTank"));
	if (TankClassFinder.Succeeded())
	{
		DefenderTankClass = TankClassFinder.Class;
	}
	else {}

	static ConstructorHelpers::FClassFinder<AZTDDefenderUnit> HeliClassFinder(TEXT("/Game/Blueprints/BP_DefenderHeli"));
	if (HeliClassFinder.Succeeded())
	{
		DefenderHeliClass = HeliClassFinder.Class;
	}
	else {}
}

void AZTDPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AZTDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Input is handled via key state polling in Tick for UE5.7 Enhanced Input compatibility
}

void AZTDPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Poll key states for input (compatible with Enhanced Input)
	// Wait a few seconds for input system to be fully initialized
	if (GetWorld() && GetWorld()->GetTimeSeconds() > 2.0f && WasInputKeyJustPressed(EKeys::R))
	{
		HandleBuildMenu();
	}
	if (WasInputKeyJustPressed(EKeys::RightMouseButton))
	{
		HandleRightClick();
	}
	if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		HandleLeftClick();
	}
	if (WasInputKeyJustPressed(EKeys::Escape))
	{
		HandleEscape();
	}
	if (WasInputKeyJustPressed(EKeys::C))
	{
		HandleContinue();
	}
	if (WasInputKeyJustPressed(EKeys::X))
	{
		HandleExit();
	}
	if (WasInputKeyJustPressed(EKeys::SpaceBar))
	{
		HandleSpace();
	}

	// Update placement preview position
	if (bIsPlacing && PlacementPreview)
	{
		FVector Location = GetPlacementLocation();
		PlacementPreview->SetActorLocation(Location);
	}
}

void AZTDPlayerController::HandleBuildMenu()
{
	AZTDGameMode* GM = GetZTDGameMode();
	if (!GM) return;

	if (GM->CurrentGameState == EZTDGameState::Paused) return;
	if (GM->CurrentGameState == EZTDGameState::GameOver) return;
	if (GM->CurrentGameState == EZTDGameState::WaveSummary) return;

	// Allow building during wave phase - no restriction for WaveInProgress

	if (bIsPlacing)
	{
		CancelBuilding();
		return;
	}

	if (bIsBuildMenuOpen)
	{
		HideBuildMenu();
	}
	else
	{
		HideUnitMenu();
		ShowBuildMenu();
	}
}

void AZTDPlayerController::HandleRightClick()
{
	// Right-click is now only for camera control, not build menu
}

void AZTDPlayerController::HandleLeftClick()
{
	AZTDGameMode* GM = GetZTDGameMode();
	if (!GM) return;

	if (GM->CurrentGameState == EZTDGameState::Paused) return;
	if (GM->CurrentGameState == EZTDGameState::GameOver) return;

	// If placing a unit, try to place it
	if (bIsPlacing)
	{
		TryPlaceUnit();
		return;
	}

	// If build menu is open, don't process clicks on world
	if (bIsBuildMenuOpen) return;

	// Try to select a defending unit
	FHitResult HitResult;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

	if (HitResult.bBlockingHit)
	{
		AZTDDefenderUnit* Defender = Cast<AZTDDefenderUnit>(HitResult.GetActor());
		if (Defender)
		{
			ShowUnitMenu(Defender);
			return;
		}
	}

	HideUnitMenu();
}

void AZTDPlayerController::HandleEscape()
{
	AZTDGameMode* GM = GetZTDGameMode();
	if (!GM) return;

	if (bIsPlacing)
	{
		CancelBuilding();
		return;
	}

	if (bIsBuildMenuOpen)
	{
		HideBuildMenu();
		return;
	}

	if (GM->CurrentGameState == EZTDGameState::Paused)
	{
		return; // Already paused, use C to resume
	}

	if (GM->CurrentGameState == EZTDGameState::WaveSummary)
	{
		GM->DismissWaveSummary();
		return;
	}

	// Just pause the game - HUD will show the pause screen
	GM->PauseGame();
}

void AZTDPlayerController::HandleContinue()
{
	AZTDGameMode* GM = GetZTDGameMode();
	if (!GM) return;

	if (GM->CurrentGameState == EZTDGameState::Paused)
	{
		GM->ResumeGame();
	}
	else if (GM->CurrentGameState == EZTDGameState::WaveSummary)
	{
		GM->DismissWaveSummary();
	}
	else if (GM->CurrentGameState == EZTDGameState::BuildPhase)
	{
		GM->ActuallyStartWave();
	}
	else if (GM->CurrentGameState == EZTDGameState::GameOver)
	{
		// Restart game
		UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), true);
	}
}

void AZTDPlayerController::HandleExit()
{
	AZTDGameMode* GM = GetZTDGameMode();
	if (!GM) return;

	GM->ExitGame();
}

void AZTDPlayerController::HandleSpace()
{
	AZTDGameMode* GM = GetZTDGameMode();
	if (!GM) return;

	// Space key is no longer used - C key handles everything
}

void AZTDPlayerController::ShowBuildMenu()
{
	if (BuildMenuWidgetClass && !BuildMenuWidget)
	{
		BuildMenuWidget = CreateWidget<UZTDBuildMenuWidget>(this, BuildMenuWidgetClass);
	}

	if (BuildMenuWidget)
	{
		BuildMenuWidget->AddToViewport(10);
		bIsBuildMenuOpen = true;
	}
}

void AZTDPlayerController::HideBuildMenu()
{
	if (BuildMenuWidget)
	{
		BuildMenuWidget->RemoveFromParent();
	}
	bIsBuildMenuOpen = false;
}

void AZTDPlayerController::SelectBuildType(EZTDBuildType Type)
{
	CurrentBuildType = Type;
	HideBuildMenu();

	if (Type == EZTDBuildType::None)
	{
		CancelBuilding();
		return;
	}

	bIsPlacing = true;

	// Create a simple placement preview actor
	if (PlacementPreview)
	{
		PlacementPreview->Destroy();
		PlacementPreview = nullptr;
	}

	// Spawn a preview actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TSubclassOf<AZTDDefenderUnit> ClassToSpawn = (Type == EZTDBuildType::Tank) ? DefenderTankClass : DefenderHeliClass;
	if (ClassToSpawn)
	{
		PlacementPreview = GetWorld()->SpawnActor<AActor>(ClassToSpawn, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (PlacementPreview)
		{
			PlacementPreview->SetActorEnableCollision(false);
			// Mark as placement preview so enemies won't target it
			AZTDDefenderUnit* DefenderPreview = Cast<AZTDDefenderUnit>(PlacementPreview);
			if (DefenderPreview)
			{
				DefenderPreview->bIsPlacementPreview = true;
			}
		}
	}
}

void AZTDPlayerController::CancelBuilding()
{
	bIsPlacing = false;
	CurrentBuildType = EZTDBuildType::None;

	if (PlacementPreview)
	{
		PlacementPreview->Destroy();
		PlacementPreview = nullptr;
	}
}

bool AZTDPlayerController::TryPlaceUnit()
{
	AZTDGameMode* GM = GetZTDGameMode();
	if (!GM) return false;

	FVector Location = GetPlacementLocation();
	if (!IsValidPlacement(Location)) return false;

	// Adjust spawn height to prevent sinking through floor
	Location.Z += 100.0f; // Spawn 100 units above ground to be safe

	TSubclassOf<AZTDDefenderUnit> ClassToSpawn = (CurrentBuildType == EZTDBuildType::Tank) ? DefenderTankClass : DefenderHeliClass;
	if (!ClassToSpawn) return false;

	// Get spawn cost from the defender class default object
	int32 Cost = 5; // Default fallback
	if (ClassToSpawn)
	{
		AZTDDefenderUnit* DefaultDefender = ClassToSpawn->GetDefaultObject<AZTDDefenderUnit>();
		if (DefaultDefender)
		{
			Cost = DefaultDefender->SpawnCost;
		}
	}
	
	if (!GM->SpendPoints(Cost)) return false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AZTDDefenderUnit* NewUnit = GetWorld()->SpawnActor<AZTDDefenderUnit>(ClassToSpawn, Location, FRotator::ZeroRotator, SpawnParams);
	if (NewUnit)
	{
		// Clear placement preview flag - this is a real unit
		NewUnit->bIsPlacementPreview = false;
		CancelBuilding();
		return true;
	}

	return false;
}

bool AZTDPlayerController::IsValidPlacement(const FVector& Location) const
{
	// Check if on the floor - make trace much longer and more forgiving
	FHitResult FloorHit;
	FVector TraceStart = Location + FVector(0, 0, 1000); // Start much higher
	FVector TraceEnd = Location - FVector(0, 0, 500); // Trace much deeper

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PlacementPreview);
	QueryParams.bTraceComplex = false;

	bool bHitFloor = GetWorld()->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams);
	
	// Debug: Show if we hit floor
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, bHitFloor ? FColor::Green : FColor::Red, 
			bHitFloor ? TEXT("Floor detected") : TEXT("No floor detected"));
	}

	if (!bHitFloor) return false;

	// Check for overlapping defenders - exclude placement preview
	float MinDistance = 100.0f; // Reduced from 150
	int32 DefenderCount = 0;
	for (TActorIterator<AZTDDefenderUnit> It(GetWorld()); It; ++It)
	{
		// Skip the placement preview to prevent self-collision
		if (*It == PlacementPreview) continue;
		
		DefenderCount++;
		float Distance = FVector::Dist((*It)->GetActorLocation(), Location);
		if (GEngine)
		{
			FString DebugStr = FString::Printf(TEXT("Defender %d at distance %.1f"), DefenderCount, Distance);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, DebugStr);
		}
		
		if (Distance < MinDistance)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Too close to other defender"));
			}
			return false;
		}
	}
	
	if (GEngine && DefenderCount == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("No other defenders found"));
	}

	return true;
}

FVector AZTDPlayerController::GetPlacementLocation() const
{
	FHitResult HitResult;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

	if (HitResult.bBlockingHit)
	{
		return HitResult.Location;
	}

	return FVector::ZeroVector;
}

void AZTDPlayerController::ShowUnitMenu(AZTDDefenderUnit* Unit)
{
	SelectedDefender = Unit;

	if (UnitMenuWidgetClass && !UnitMenuWidget)
	{
		UnitMenuWidget = CreateWidget<UZTDUnitMenuWidget>(this, UnitMenuWidgetClass);
	}

	if (UnitMenuWidget)
	{
		UnitMenuWidget->SetUnit(Unit);
		UnitMenuWidget->AddToViewport(10);
	}
}

void AZTDPlayerController::HideUnitMenu()
{
	if (UnitMenuWidget)
	{
		UnitMenuWidget->RemoveFromParent();
	}
	SelectedDefender = nullptr;
}

void AZTDPlayerController::ShowPauseMenu()
{
	if (PauseMenuWidgetClass && !PauseMenuWidget)
	{
		PauseMenuWidget = CreateWidget<UZTDPauseMenuWidget>(this, PauseMenuWidgetClass);
	}

	if (PauseMenuWidget)
	{
		PauseMenuWidget->AddToViewport(20);
	}
	bIsPaused = true;
}

void AZTDPlayerController::HidePauseMenu()
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
	}
	bIsPaused = false;
}

void AZTDPlayerController::ShowWaveSummary(int32 WaveNumber, int32 EnemiesDestroyed, float BaseHP, int32 Points)
{
	if (WaveSummaryWidgetClass && !WaveSummaryWidget)
	{
		WaveSummaryWidget = CreateWidget<UZTDWaveSummaryWidget>(this, WaveSummaryWidgetClass);
	}

	if (WaveSummaryWidget)
	{
		WaveSummaryWidget->UpdateSummary(WaveNumber, EnemiesDestroyed, BaseHP, Points);
		WaveSummaryWidget->AddToViewport(20);
	}
	bIsWaveSummaryOpen = true;
}

void AZTDPlayerController::HideWaveSummary()
{
	if (WaveSummaryWidget)
	{
		WaveSummaryWidget->RemoveFromParent();
	}
	bIsWaveSummaryOpen = false;
}

void AZTDPlayerController::ShowGameOver(int32 WavesCompleted, int32 TotalKills, int32 FinalPoints)
{
	if (GameOverWidgetClass && !GameOverWidget)
	{
		GameOverWidget = CreateWidget<UZTDGameOverWidget>(this, GameOverWidgetClass);
	}

	if (GameOverWidget)
	{
		GameOverWidget->UpdateGameOver(WavesCompleted, TotalKills, FinalPoints);
		GameOverWidget->AddToViewport(20);
	}
}

void AZTDPlayerController::HideGameOver()
{
	if (GameOverWidget)
	{
		GameOverWidget->RemoveFromParent();
	}
}

void AZTDPlayerController::ShowInstructions()
{
	if (InstructionsWidgetClass && !InstructionsWidget)
	{
		InstructionsWidget = CreateWidget<UZTDInstructionsWidget>(this, InstructionsWidgetClass);
	}

	if (InstructionsWidget)
	{
		InstructionsWidget->AddToViewport(10);
		InstructionsWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AZTDPlayerController::HideInstructions()
{
	if (InstructionsWidget)
	{
		InstructionsWidget->RemoveFromParent();
	}
}


AZTDGameMode* AZTDPlayerController::GetZTDGameMode() const
{
	return Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}
