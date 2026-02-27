#include "ZTDPauseMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"

void UZTDPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree) return;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.7f));
	Border->SetPadding(FMargin(20.0f));
	Canvas->AddChild(Border);
	if (UCanvasPanelSlot* PSlot = Cast<UCanvasPanelSlot>(Border->Slot))
	{
		PSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		PSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		PSlot->SetSize(FVector2D(300.0f, 150.0f));
		PSlot->SetPosition(FVector2D(0.0f, 0.0f));
	}

	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
	Title->SetText(FText::FromString(TEXT("GAME PAUSED")));
	Title->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	Title->SetJustification(ETextJustify::Center);
	VBox->AddChild(Title);

	UTextBlock* ContText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContText"));
	ContText->SetText(FText::FromString(TEXT("Press C to Continue")));
	ContText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ContText->SetJustification(ETextJustify::Center);
	VBox->AddChild(ContText);

	UTextBlock* ExText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ExText"));
	ExText->SetText(FText::FromString(TEXT("Press X to Exit")));
	ExText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ExText->SetJustification(ETextJustify::Center);
	VBox->AddChild(ExText);
}
