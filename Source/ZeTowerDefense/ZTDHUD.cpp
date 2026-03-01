#include "ZTDHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AZTDHUD::AZTDHUD()
{
	// Load the default Roboto font used by UE HUD
	static ConstructorHelpers::FObjectFinder<UFont> FontObj(TEXT("/Engine/EngineFonts/Roboto"));
	if (FontObj.Succeeded())
	{
		HUDFont = FontObj.Object;
	}
}

void AZTDHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AZTDHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas) return;

	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	// Update screen dimensions
	ScreenWidth = Canvas->SizeX;
	ScreenHeight = Canvas->SizeY;

	// Draw game state screens (wave pause, pause, game over, wave summary)
	if (GM->CurrentGameState != EZTDGameState::WaveInProgress)
	{
		DrawGameScreen();
		return;
	}

	// Draw normal HUD elements during gameplay
	DrawGameplayHUD();
	// DrawCrosshair(); // Removed crosshair
	DrawWaveInfo();
}

void AZTDHUD::DrawGameScreen()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	// Dark overlay (filled rectangle covering entire screen)
	FLinearColor OverlayColor(0.0f, 0.0f, 0.0f, 0.85f);
	FCanvasTileItem OverlayTile(FVector2D(0.0f, 0.0f), FVector2D(ScreenWidth, ScreenHeight), OverlayColor);
	OverlayTile.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(OverlayTile);

	float CX = ScreenWidth * 0.5f;
	float CY = ScreenHeight * 0.5f;

	FLinearColor TitleColor(1.0f, 0.9f, 0.2f, 1.0f); // Yellow
	FLinearColor TextColor(0.9f, 0.9f, 0.9f, 0.9f); // White
	FLinearColor PromptColor(0.3f, 1.0f, 0.4f, 1.0f); // Green

	if (GM->CurrentGameState == EZTDGameState::BuildPhase)
	{
		// Wave start screen - left-aligned with big margin
		float LeftMargin = ScreenWidth * 0.15f; // 15% from left edge
		
		DrawLeftAlignedText(TEXT("ZE TOWER DEFENSE"), LeftMargin, CY - 250.0f, TitleColor, 2.8f);
		
		// Game description
		DrawLeftAlignedText(TEXT("Defend your base from enemy waves!"), LeftMargin, CY - 180.0f, TextColor, 1.3f);
		
		// Camera controls section
		DrawLeftAlignedText(TEXT("CAMERA CONTROLS"), LeftMargin, CY - 130.0f, FLinearColor(0.8f, 0.8f, 1.0f, 1.0f), 1.1f);
		DrawLeftAlignedText(TEXT("A, D - Move camera Left/Right"), LeftMargin, CY - 100.0f, TextColor, 1.0f);
		DrawLeftAlignedText(TEXT("W, S - Move camera Forward/Backward"), LeftMargin, CY - 75.0f, TextColor, 1.0f);
		
		// Building section
		DrawLeftAlignedText(TEXT("BUILDING"), LeftMargin, CY - 25.0f, FLinearColor(0.8f, 0.8f, 1.0f, 1.0f), 1.1f);
		DrawLeftAlignedText(TEXT("Use the Build Menu at bottom left to build defenses"), LeftMargin, CY, TextColor, 1.0f);
		
		// Wave info section
		FString WaveText = FString::Printf(TEXT("WAVE %d"), GM->CurrentWaveNumber + 1);
		DrawLeftAlignedText(WaveText, LeftMargin, CY + 40.0f, TextColor, 1.8f);
		
		int32 TankCount = GM->WaveSpawner ? GM->WaveSpawner->GetTankCountForWave(GM->CurrentWaveNumber + 1) : 0;
		int32 HeliCount = GM->WaveSpawner ? GM->WaveSpawner->GetHeliCountForWave(GM->CurrentWaveNumber + 1) : 0;
		FString EnemyText = FString::Printf(TEXT("Incoming: %d Tanks, %d Helis"), TankCount, HeliCount);
		DrawLeftAlignedText(EnemyText, LeftMargin, CY + 70.0f, TextColor, 1.2f);
		
		// Start prompt - centered horizontally
		DrawCenteredText(TEXT("Press C or Space to start the wave"), CY + 220.0f, PromptColor, 1.4f);
		DrawCenteredText(TEXT("Press X to close game"), CY + 250.0f, PromptColor, 1.4f);
	}
	else if (GM->CurrentGameState == EZTDGameState::Paused)
	{
		// Show exactly the same as wave start screen
		float LeftMargin = ScreenWidth * 0.15f; // 15% from left edge
		
		DrawLeftAlignedText(TEXT("ZE TOWER DEFENSE"), LeftMargin, CY - 250.0f, TitleColor, 2.8f);
		
		// Game description
		DrawLeftAlignedText(TEXT("Defend your base from enemy waves!"), LeftMargin, CY - 180.0f, TextColor, 1.3f);
		
		// Camera controls section
		DrawLeftAlignedText(TEXT("CAMERA CONTROLS"), LeftMargin, CY - 130.0f, FLinearColor(0.8f, 0.8f, 1.0f, 1.0f), 1.1f);
		DrawLeftAlignedText(TEXT("A, D - Move camera Left/Right"), LeftMargin, CY - 100.0f, TextColor, 1.0f);
		DrawLeftAlignedText(TEXT("W, S - Move camera Forward/Backward"), LeftMargin, CY - 75.0f, TextColor, 1.0f);
		
		// Building section
		DrawLeftAlignedText(TEXT("BUILDING"), LeftMargin, CY - 25.0f, FLinearColor(0.8f, 0.8f, 1.0f, 1.0f), 1.1f);
		DrawLeftAlignedText(TEXT("Use the Build Menu at bottom left to build defenses"), LeftMargin, CY, TextColor, 1.0f);
		
		// Wave info section (show current wave info like wave start)
		FString WaveText = FString::Printf(TEXT("WAVE %d"), GM->CurrentWaveNumber + 1);
		DrawLeftAlignedText(WaveText, LeftMargin, CY + 40.0f, TextColor, 1.8f);
		
		int32 TankCount = GM->WaveSpawner ? GM->WaveSpawner->GetTankCountForWave(GM->CurrentWaveNumber + 1) : 0;
		int32 HeliCount = GM->WaveSpawner ? GM->WaveSpawner->GetHeliCountForWave(GM->CurrentWaveNumber + 1) : 0;
		FString EnemyText = FString::Printf(TEXT("Incoming: %d Tanks, %d Helis"), TankCount, HeliCount);
		DrawLeftAlignedText(EnemyText, LeftMargin, CY + 70.0f, TextColor, 1.2f);
		
		// Resume prompt - centered horizontally
		DrawCenteredText(TEXT("Press C or Space to continue"), CY + 220.0f, PromptColor, 1.4f);
		DrawCenteredText(TEXT("Press X to close game"), CY + 250.0f, PromptColor, 1.4f);
	}
	else if (GM->CurrentGameState == EZTDGameState::GameOver)
	{
		// Game over screen
		DrawCenteredText(TEXT("GAME OVER"), CY - 80.0f, FLinearColor(1.0f, 0.3f, 0.3f, 1.0f), 3.0f);
		DrawCenteredText(TEXT("Your base has been destroyed!"), CY - 20.0f, TextColor, 1.2f);
		DrawCenteredText(FString::Printf(TEXT("Waves survived: %d"), GM->CurrentWaveNumber), CY + 30.0f, TextColor, 1.0f);
		DrawCenteredText(TEXT("Press C to restart"), CY + 80.0f, PromptColor, 1.3f);
	}
	else if (GM->CurrentGameState == EZTDGameState::WaveSummary)
	{
		// Wave summary screen
		FString WaveText = FString::Printf(TEXT("WAVE %d COMPLETE!"), GM->CurrentWaveNumber);
		DrawCenteredText(WaveText, CY - 80.0f, TitleColor, 2.0f);
		DrawCenteredText(TEXT("Wave completed!"), CY - 20.0f, TextColor, 1.2f);
		DrawCenteredText(TEXT("Press C for next wave"), CY + 40.0f, PromptColor, 1.3f);
	}
}

void AZTDHUD::DrawGameplayHUD()
{
	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM || !HUDFont) return;

	// Draw dark background panel for HUD info
	FVector2D PanelPos(10.0f, 10.0f);
	FVector2D PanelSize(125.0f, 140.0f); // Reduced width by half
	FLinearColor PanelColor(0.0f, 0.0f, 0.0f, 0.7f); // Dark semi-transparent
	DrawBox(PanelPos, PanelSize, PanelColor);

	// Consistent text color for all HUD elements
	FLinearColor HUDTextColor(0.9f, 0.9f, 0.9f, 1.0f); // White
	FLinearColor WarningColor(1.0f, 0.4f, 0.4f, 1.0f); // Red for low HP

	// Draw base HP (closer spacing)
	float BaseHP = GM->GetBaseHP();
	FString HPText = FString::Printf(TEXT("Base HP: %.0f"), BaseHP);
	FLinearColor HPColor = (BaseHP < 25.0f) ? WarningColor : HUDTextColor; // Warning when below 25 HP
	DrawLeftAlignedText(HPText, 20.0f, 20.0f, HPColor, 1.2f);

	// Draw points (closer spacing)
	FString PointsText = FString::Printf(TEXT("Points: %d"), GM->GetPlayerPoints());
	DrawLeftAlignedText(PointsText, 20.0f, 45.0f, HUDTextColor, 1.2f);

	// Draw wave number (closer spacing)
	FString WaveText = FString::Printf(TEXT("Wave: %d"), GM->CurrentWaveNumber);
	DrawLeftAlignedText(WaveText, 20.0f, 70.0f, HUDTextColor, 1.2f);

	// Draw enemy counters
	if (GM->WaveSpawner)
	{
		int32 TotalTanks = GM->WaveSpawner->GetTankCountForWave(GM->CurrentWaveNumber);
		int32 TotalHelis = GM->WaveSpawner->GetHeliCountForWave(GM->CurrentWaveNumber);
		int32 TanksKilled = GM->WaveSpawner->GetTanksKilledThisWave();
		int32 HelisKilled = GM->WaveSpawner->GetHelisKilledThisWave();

		FString TankText = FString::Printf(TEXT("Tank: %d/%d"), TanksKilled, TotalTanks);
		DrawLeftAlignedText(TankText, 20.0f, 95.0f, HUDTextColor, 1.2f);

		FString HeliText = FString::Printf(TEXT("Heli: %d/%d"), HelisKilled, TotalHelis);
		DrawLeftAlignedText(HeliText, 20.0f, 120.0f, HUDTextColor, 1.2f);
	}
}

void AZTDHUD::DrawCrosshair()
{
	// Simple crosshair at center
	float CX = ScreenWidth * 0.5f;
	float CY = ScreenHeight * 0.5f;
	float Size = 10.0f;
	float Thickness = 2.0f;
	FLinearColor Color(1.0f, 1.0f, 1.0f, 0.8f);

	// Horizontal line
	Canvas->K2_DrawLine(FVector2D(CX - Size, CY), FVector2D(CX + Size, CY), Thickness, Color);
	// Vertical line
	Canvas->K2_DrawLine(FVector2D(CX, CY - Size), FVector2D(CX, CY + Size), Thickness, Color);
}

void AZTDHUD::DrawWaveInfo()
{
	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM || !HUDFont) return;

	// Draw current wave info at top right
	FString WaveText = FString::Printf(TEXT("Wave %d"), GM->CurrentWaveNumber);
	DrawCenteredText(WaveText, 50.0f, FLinearColor(0.9f, 0.9f, 0.9f, 1.0f), 1.2f);
}

void AZTDHUD::DrawCenteredText(const FString& Text, float Y, FLinearColor Color, float Scale)
{
	if (!HUDFont || !Canvas) return;

	float CX = ScreenWidth * 0.5f;

	// Approximate text width for centering
	float CharW = 10.0f * Scale;
	float TextW = Text.Len() * CharW;

	FCanvasTextItem Item(FVector2D(CX - TextW * 0.5f, Y), FText::FromString(Text), HUDFont, Color);
	Item.Scale = FVector2D(Scale, Scale);
	Item.bOutlined = true;
	Item.OutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.8f);
	Canvas->DrawItem(Item);
}

void AZTDHUD::DrawLeftAlignedText(const FString& Text, float X, float Y, FLinearColor Color, float Scale)
{
	if (!HUDFont || !Canvas) return;

	FCanvasTextItem Item(FVector2D(X, Y), FText::FromString(Text), HUDFont, Color);
	Item.Scale = FVector2D(Scale, Scale);
	Item.bOutlined = true;
	Item.OutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.8f);
	Canvas->DrawItem(Item);
}

void AZTDHUD::DrawBox(FVector2D Position, FVector2D Size, FLinearColor Color)
{
	if (!Canvas) return;

	FCanvasTileItem TileItem(Position, Size, Color);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}
