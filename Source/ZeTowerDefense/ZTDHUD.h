#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ZTDGameMode.h"
#include "ZTDPlayerController.h"
#include "ZTDBase.h"
#include "ZTDWaveSpawner.h"
#include "ZTDHUD.generated.h"

UCLASS()
class ZETOWERDEFENSE_API AZTDHUD : public AHUD
{
	GENERATED_BODY()

public:
	AZTDHUD();

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	// Draw functions
	void DrawGameScreen();
	void DrawGameplayHUD();
	void DrawCrosshair();
	void DrawBuildMenu();
	void DrawWaveInfo();

	// Helper functions
	void DrawCenteredText(const FString& Text, float Y, FLinearColor Color, float Scale = 1.0f);
	void DrawLeftAlignedText(const FString& Text, float X, float Y, FLinearColor Color, float Scale = 1.0f);
	void DrawBox(FVector2D Position, FVector2D Size, FLinearColor Color);

private:
	UPROPERTY()
	UFont* HUDFont = nullptr;

	// Screen dimensions
	float ScreenWidth = 0.0f;
	float ScreenHeight = 0.0f;
};
