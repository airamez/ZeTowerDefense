#include "ZTDGameOverWidget.h"
#include "../ZTDGameMode.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UZTDGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree) return;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.2f, 0.0f, 0.0f, 0.9f));
	Border->SetPadding(FMargin(20.0f));
	Canvas->AddChild(Border);
	if (UCanvasPanelSlot* PSlot = Cast<UCanvasPanelSlot>(Border->Slot))
	{
		PSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		PSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		PSlot->SetSize(FVector2D(350.0f, 250.0f));
		PSlot->SetPosition(FVector2D(0.0f, 0.0f));
	}

	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	GameOverTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("GOTitle"));
	GameOverTitle->SetText(FText::FromString(TEXT("GAME OVER")));
	GameOverTitle->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	GameOverTitle->SetJustification(ETextJustify::Center);
	VBox->AddChild(GameOverTitle);

	WavesText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Waves"));
	WavesText->SetText(FText::FromString(TEXT("Waves: 0")));
	WavesText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(WavesText);

	KillsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Kills"));
	KillsText->SetText(FText::FromString(TEXT("Kills: 0")));
	KillsText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(KillsText);

	PointsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Points"));
	PointsText->SetText(FText::FromString(TEXT("Score: 0")));
	PointsText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(PointsText);

	ExitButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ExitBtn"));
	VBox->AddChild(ExitButton);
	if (UVerticalBoxSlot* BSlot = Cast<UVerticalBoxSlot>(ExitButton->Slot))
	{
		BSlot->SetPadding(FMargin(0.0f, 10.0f, 0.0f, 0.0f));
	}
	UTextBlock* ExitTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ExitTxt"));
	ExitTxt->SetText(FText::FromString(TEXT("Exit Game")));
	ExitTxt->SetJustification(ETextJustify::Center);
	ExitButton->AddChild(ExitTxt);

	ExitButton->OnClicked.AddDynamic(this, &UZTDGameOverWidget::OnExitClicked);
}

void UZTDGameOverWidget::UpdateGameOver(int32 WavesCompleted, int32 TotalKills, int32 FinalPoints)
{
	if (WavesText)
	{
		WavesText->SetText(FText::FromString(FString::Printf(TEXT("Waves Completed: %d"), WavesCompleted)));
	}
	if (KillsText)
	{
		KillsText->SetText(FText::FromString(FString::Printf(TEXT("Total Kills: %d"), TotalKills)));
	}
	if (PointsText)
	{
		PointsText->SetText(FText::FromString(FString::Printf(TEXT("Final Score: %d"), FinalPoints)));
	}
}

void UZTDGameOverWidget::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}
