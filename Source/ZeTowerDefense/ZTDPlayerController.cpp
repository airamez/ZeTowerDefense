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
	WaveSummaryWidgetClass = UZTDWaveSummaryWidget::StaticClass();
	GameOverWidgetClass = UZTDGameOverWidget::StaticClass();
	WavePauseWidgetClass = UZTDWavePauseWidget::StaticClass();
	InstructionsWidgetClass = UZTDInstructionsWidget::StaticClass();

	// Try to find Blueprint build menu widget
	static ConstructorHelpers::FClassFinder<UUserWidget> BuildMenuBPClassFinder(TEXT("/Game/WBP_BuildMenu"));
	if (BuildMenuBPClassFinder.Succeeded())
	{
		BuildMenuBlueprintClass = BuildMenuBPClassFinder.Class;
		UE_LOG(LogTemp, Warning, TEXT("Found Blueprint WBP_BuildMenu class in constructor"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Blueprint WBP_BuildMenu not found in constructor"));
	}

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

	// Show initial instructions for new game
	ShowInstructions();
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
	// Build menu is always visible, nothing to toggle
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

	// Try to select a defending unit with more forgiving click detection
	FHitResult HitResult;
	
	// First try line trace for precision
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

	// If line trace didn't hit a defender, try sphere trace for nearby units
	if (!HitResult.bBlockingHit || !Cast<AZTDDefenderUnit>(HitResult.GetActor()))
	{
		// Get cursor world position
		FVector CursorWorldLocation, CursorWorldDirection;
		DeprojectMousePositionToWorld(CursorWorldLocation, CursorWorldDirection);
		
		// Sphere trace around cursor position (100 units radius)
		FVector TraceStart = CursorWorldLocation;
		FVector TraceEnd = CursorWorldLocation + CursorWorldDirection * 1000.0f;
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		
		FHitResult SphereHitResult;
		bool bHit = GetWorld()->SweepSingleByChannel(
			SphereHitResult,
			TraceStart,
			TraceEnd,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility, // Use ECollisionChannel directly
			FCollisionShape::MakeSphere(200.0f), // 200 units radius for more forgiving detection
			QueryParams
		);
		
		if (bHit)
		{
			HitResult = SphereHitResult;
		}
	}

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

	if (GM->CurrentGameState == EZTDGameState::Paused)
	{
		return; // Already paused, use C to resume
	}

	if (GM->CurrentGameState == EZTDGameState::WaveSummary)
	{
		GM->DismissWaveSummary();
		return;
	}

	// Pause the game to show HUD instructions screen
	GM->PauseGame();
}

void AZTDPlayerController::HandleContinue()
{
	AZTDGameMode* GM = GetZTDGameMode();
	if (!GM) return;

	// Check if we're dismissing instructions (opening screen)
	if (InstructionsWidget && InstructionsWidget->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("Dismissing instructions, starting wave directly"));
		HideInstructions();
		ShowBuildMenu();
		// Start the wave directly on first C press
		if (GM->CurrentGameState == EZTDGameState::BuildPhase)
		{
			GM->ActuallyStartWave();
		}
		return;
	}

	// Also create build menu when entering BuildPhase
	if (GM->CurrentGameState == EZTDGameState::BuildPhase && !BuildMenuWidget && (!InstructionsWidget || !InstructionsWidget->IsInViewport()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Entering BuildPhase, creating build menu"));
		ShowBuildMenu();
	}

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
	// Space key now works as continue (same as C key)
	HandleContinue();
}

void AZTDPlayerController::ShowBuildMenu()
{
	// Don't show build menu if first unit has already been placed
	if (bFirstUnitPlaced)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skipping build menu show - first unit already placed"));
		return;
	}

	// Use Blueprint class if available, otherwise fall back to programmatic
	TSubclassOf<UUserWidget> WidgetClassToUse = BuildMenuBlueprintClass ? BuildMenuBlueprintClass : BuildMenuWidgetClass;
	
	if (WidgetClassToUse && !BuildMenuWidget)
	{
		BuildMenuWidget = CreateWidget<UUserWidget>(this, WidgetClassToUse);
		UE_LOG(LogTemp, Warning, TEXT("BuildMenuWidget created"));
	}

	if (BuildMenuWidget && !BuildMenuWidget->IsInViewport())
	{
		// Add a 2-second delay to avoid any initialization clearing
		FTimerHandle DelayTimer;
		GetWorld()->GetTimerManager().SetTimer(DelayTimer, [this]()
		{
			if (BuildMenuWidget && !bFirstUnitPlaced) // Check flag again in case unit was placed during delay
			{
				// Try multiple methods to show the widget
				UE_LOG(LogTemp, Warning, TEXT("Attempting to show build menu with multiple methods"));
				
				// Method 1: Add to viewport with maximum z-order
				BuildMenuWidget->AddToViewport(9999);
				bIsBuildMenuOpen = true;
				UE_LOG(LogTemp, Warning, TEXT("BuildMenuWidget added to viewport with z-order 9999"));
				
				// Method 2: Force visibility multiple times
				for (int32 i = 0; i < 5; i++)
				{
					BuildMenuWidget->SetVisibility(ESlateVisibility::Visible);
				}
				
				// Method 3: Try to add to viewport again after a short delay
				FTimerHandle RetryTimer;
				GetWorld()->GetTimerManager().SetTimer(RetryTimer, [this]()
				{
					if (BuildMenuWidget && !bFirstUnitPlaced) // Check flag again
					{
						BuildMenuWidget->RemoveFromViewport();
						BuildMenuWidget->AddToViewport(9999);
						BuildMenuWidget->SetVisibility(ESlateVisibility::Visible);
						UE_LOG(LogTemp, Warning, TEXT("Retry: Re-added BuildMenuWidget to viewport"));
					}
				}, 0.1f, false);
				
				// Method 4: Add a persistent visibility timer
				GetWorld()->GetTimerManager().SetTimer(BuildMenuVisibilityTimer, [this]()
				{
					if (BuildMenuWidget && BuildMenuWidget->IsInViewport() && !bFirstUnitPlaced) // Check flag every time
					{
						// Force visibility every frame
						BuildMenuWidget->SetVisibility(ESlateVisibility::Visible);
					}
					else if (bFirstUnitPlaced && BuildMenuWidget && BuildMenuWidget->IsInViewport())
					{
						// Stop trying to show build menu and clear the timer
						UE_LOG(LogTemp, Warning, TEXT("First unit placed, stopping build menu visibility timer"));
						GetWorld()->GetTimerManager().ClearTimer(BuildMenuVisibilityTimer);
					}
				}, 0.1f, true); // Every 100ms, persistent
			}
		}, 2.0f, false); // 2-second delay
	}
}

void AZTDPlayerController::HideBuildMenu()
{
	// Build menu is always visible - do not remove
}

void AZTDPlayerController::SelectBuildType(EZTDBuildType Type)
{
	CurrentBuildType = Type;

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
		
		// Set flag that first unit has been placed
		if (!bFirstUnitPlaced)
		{
			bFirstUnitPlaced = true;
			UE_LOG(LogTemp, Warning, TEXT("First unit placed, stopping build menu visibility attempts"));
		}
		
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
	
	if (!bHitFloor) return false;

	// Check for overlapping defenders - exclude placement preview
	float MinDistance = 300.0f; // Increased significantly to prevent overlapping
	
	for (TActorIterator<AZTDDefenderUnit> It(GetWorld()); It; ++It)
	{
		AZTDDefenderUnit* Defender = *It;
		
		// Skip the placement preview to prevent self-collision
		if (Defender == PlacementPreview) continue;
		
		// Skip placement previews entirely
		if (Defender->bIsPlacementPreview) continue;
		
		float Distance = FVector::Dist(Defender->GetActorLocation(), Location);
		if (Distance < MinDistance)
		{
			return false; // Too close to existing defender
		}
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
