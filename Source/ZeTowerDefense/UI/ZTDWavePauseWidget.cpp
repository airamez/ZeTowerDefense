#include "ZTDWavePauseWidget.h"
#include "../ZTDGameMode.h"
#include "../ZTDPlayerController.h"
#include "ZTDUIStrings.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Styling/SlateBrush.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void UZTDWavePauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree) 
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Pause Widget: WidgetTree is null!"));
		return;
	}

	// Root canvas
	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	// Background border centered - larger and more visible
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f)); // Solid black background
	Border->SetPadding(FMargin(30.0f)); // Larger padding
	Canvas->AddChild(Border);

	UCanvasPanelSlot* BorderSlot = Cast<UCanvasPanelSlot>(Border->Slot);
	if (BorderSlot)
	{
		BorderSlot->SetAnchors(FAnchors(0.5f)); // Center
		BorderSlot->SetOffsets(FMargin(-320.0f, -280.0f, 320.0f, 280.0f)); // 640x560 centered
	}

	// Vertical box inside border
	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	// Helper lambda: create text with center alignment
	auto MakeText = [&](const FString& Name, const FString& Label) -> UTextBlock*
	{
		UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *Name);
		Txt->SetText(FText::FromString(Label));
		Txt->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24)); // Larger, bold font
		Txt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Txt->SetJustification(ETextJustify::Center);
		VBox->AddChild(Txt);
		return Txt;
	};

	// Helper lambda: create button with text
	auto MakeButton = [&](const FString& Name, const FString& Label) -> UButton*
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *Name);
		VBox->AddChild(Btn);
		UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(Name + TEXT("_Text")));
		Txt->SetText(FText::FromString(Label));
		Txt->SetJustification(ETextJustify::Center);
		Btn->AddChild(Txt);
		return Btn;
	};

	// Game title - always "ZE TOWER DEFENSE", never changes
	UTextBlock* GameTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("GameTitle"));
	GameTitle->SetText(FText::FromString(TEXT("ZE TOWER DEFENSE")));
	GameTitle->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 36));
	GameTitle->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	GameTitle->SetJustification(ETextJustify::Center);
	VBox->AddChild(GameTitle);

	// Subtitle: shows "WAVE 1" or "PAUSED" - this is TitleText
	TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Subtitle"));
	TitleText->SetText(FText::FromString(TEXT("WAVE 1")));
	TitleText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 28));
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	TitleText->SetJustification(ETextJustify::Center);
	VBox->AddChild(TitleText);

	// Instructions text
	UTextBlock* InstrText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InstrText"));
	InstrText->SetText(FText::FromString(
		TEXT("Defend your base from enemy waves!\n")
		TEXT("R - Build Menu  |  Click - Place  |  ESC - Pause")
	));
	InstrText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16));
	InstrText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)));
	InstrText->SetJustification(ETextJustify::Center);
	InstrText->SetAutoWrapText(true);
	VBox->AddChild(InstrText);

	// Spacer
	UTextBlock* Spacer1 = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Spacer1"));
	Spacer1->SetText(FText::FromString(TEXT(" ")));
	VBox->AddChild(Spacer1);

	// Wave number
	WaveNumberText = MakeText(TEXT("WaveNumber"), TEXT("Wave 1"));

	// Enemy counts
	TankCountText = MakeText(TEXT("TankCount"), TEXT("Tanks: 5"));
	HeliCountText = MakeText(TEXT("HeliCount"), TEXT("Helis: 2"));

	// Spacer
	UTextBlock* Spacer2 = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Spacer2"));
	Spacer2->SetText(FText::FromString(TEXT(" ")));
	VBox->AddChild(Spacer2);

	// Start hint text
	StartHintText = MakeText(TEXT("StartHint"), TEXT("Press C to start the wave"));

	// Start button
	StartButton = MakeButton(TEXT("StartButton"), TEXT("Start Wave"));

	// Bind button event
	StartButton->OnClicked.AddDynamic(this, &UZTDWavePauseWidget::OnStartClicked);
}

void UZTDWavePauseWidget::UpdateWaveInfo(int32 WaveNumber, int32 TankCount, int32 HeliCount)
{
	CurrentWaveNumber = WaveNumber;

	if (TitleText)
	{
		TitleText->SetText(FText::FromString(FString::Printf(TEXT("WAVE %d"), WaveNumber)));
	}

	if (WaveNumberText)
	{
		WaveNumberText->SetText(FText::FromString(FString::Printf(TEXT("Enemies incoming: %d Tanks, %d Helis"), TankCount, HeliCount)));
	}

	if (TankCountText)
	{
		TankCountText->SetText(FText::FromString(TEXT("")));
	}

	if (HeliCountText)
	{
		HeliCountText->SetText(FText::FromString(TEXT("")));
	}

	if (StartHintText)
	{
		StartHintText->SetText(FText::FromString(TEXT("Press C or click Start to begin")));
	}

	if (StartButton)
	{
		StartButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UZTDWavePauseWidget::ShowAsPaused()
{
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("PAUSED")));
	}

	if (WaveNumberText)
	{
		WaveNumberText->SetText(FText::FromString(TEXT("")));
	}

	if (TankCountText)
	{
		TankCountText->SetText(FText::FromString(TEXT("")));
	}

	if (HeliCountText)
	{
		HeliCountText->SetText(FText::FromString(TEXT("")));
	}

	if (StartHintText)
	{
		StartHintText->SetText(FText::FromString(TEXT("Press C to Resume  |  X to Quit")));
	}

	if (StartButton)
	{
		StartButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UZTDWavePauseWidget::OnStartClicked()
{
	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->ActuallyStartWave();
	}
}
