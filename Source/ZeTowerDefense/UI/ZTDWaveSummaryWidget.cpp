#include "ZTDWaveSummaryWidget.h"
#include "../ZTDGameMode.h"
#include "../ZTDPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void UZTDWaveSummaryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree) return;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.1f, 0.9f));
	Border->SetPadding(FMargin(20.0f));
	Canvas->AddChild(Border);
	if (UCanvasPanelSlot* PSlot = Cast<UCanvasPanelSlot>(Border->Slot))
	{
		PSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		PSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		PSlot->SetSize(FVector2D(350.0f, 280.0f));
		PSlot->SetPosition(FVector2D(0.0f, 0.0f));
	}

	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
	TitleText->SetText(FText::FromString(TEXT("WAVE COMPLETE!")));
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	TitleText->SetJustification(ETextJustify::Center);
	VBox->AddChild(TitleText);

	WaveNumberText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("WaveNum"));
	WaveNumberText->SetText(FText::FromString(TEXT("Wave: 0")));
	WaveNumberText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(WaveNumberText);

	EnemiesDestroyedText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Enemies"));
	EnemiesDestroyedText->SetText(FText::FromString(TEXT("Enemies Destroyed: 0")));
	EnemiesDestroyedText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(EnemiesDestroyedText);

	BaseHPText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BaseHP"));
	BaseHPText->SetText(FText::FromString(TEXT("Base HP: 100")));
	BaseHPText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(BaseHPText);

	PointsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Points"));
	PointsText->SetText(FText::FromString(TEXT("Points: 0")));
	PointsText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(PointsText);

	ContinueButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ContBtn"));
	VBox->AddChild(ContinueButton);
	if (UVerticalBoxSlot* BSlot = Cast<UVerticalBoxSlot>(ContinueButton->Slot))
	{
		BSlot->SetPadding(FMargin(0.0f, 10.0f, 0.0f, 0.0f));
	}
	UTextBlock* ContTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContTxt"));
	ContTxt->SetText(FText::FromString(TEXT("Continue (or press C)")));
	ContTxt->SetJustification(ETextJustify::Center);
	ContinueButton->AddChild(ContTxt);

	ContinueButton->OnClicked.AddDynamic(this, &UZTDWaveSummaryWidget::OnContinueClicked);
}

void UZTDWaveSummaryWidget::UpdateSummary(int32 WaveNumber, int32 EnemiesDestroyed, float BaseHP, int32 Points)
{
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("WAVE COMPLETE!")));
	}
	if (WaveNumberText)
	{
		WaveNumberText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), WaveNumber)));
	}
	if (EnemiesDestroyedText)
	{
		EnemiesDestroyedText->SetText(FText::FromString(FString::Printf(TEXT("Enemies Destroyed: %d"), EnemiesDestroyed)));
	}
	if (BaseHPText)
	{
		BaseHPText->SetText(FText::FromString(FString::Printf(TEXT("Base HP: %.0f"), BaseHP)));
	}
	if (PointsText)
	{
		PointsText->SetText(FText::FromString(FString::Printf(TEXT("Points: %d"), Points)));
	}
}

void UZTDWaveSummaryWidget::OnContinueClicked()
{
	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->DismissWaveSummary();
	}

	AZTDPlayerController* PC = Cast<AZTDPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->HideWaveSummary();
	}
}
