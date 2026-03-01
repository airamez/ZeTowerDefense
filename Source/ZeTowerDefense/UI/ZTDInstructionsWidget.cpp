#include "ZTDInstructionsWidget.h"
#include "../ZTDGameMode.h"
#include "../ZTDPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void UZTDInstructionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree) return;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.92f));
	Border->SetPadding(FMargin(30.0f));
	Canvas->AddChild(Border);

	if (UCanvasPanelSlot* PSlot = Cast<UCanvasPanelSlot>(Border->Slot))
	{
		PSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		PSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		PSlot->SetSize(FVector2D(600.0f, 480.0f));
		PSlot->SetPosition(FVector2D(0.0f, 0.0f));
	}

	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
	TitleText->SetText(FText::FromString(TEXT("ZE TOWER DEFENSE")));
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	TitleText->SetJustification(ETextJustify::Center);
	VBox->AddChild(TitleText);

	InstructionsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Instructions"));
	InstructionsText->SetText(FText::FromString(
		TEXT("Defend your base from waves of enemies!\n\n")
		TEXT("CONTROLS:\n")
		TEXT("  Click   - Place Defender\n")
		TEXT("  RClick  - Cancel Building\n")
		TEXT("  ESC     - Pause Game\n")
		TEXT("  C       - Continue/Start Wave\n\n")
		TEXT("GAMEPLAY:\n")
		TEXT("  Build defenders using the always-visible build menu.\n")
		TEXT("  Press C to start the wave when ready.\n")
		TEXT("  Earn points by destroying enemies.\n")
		TEXT("  Don't let enemies reach your base!\n\n")
		TEXT("Good luck, Commander!")
	));
	InstructionsText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	InstructionsText->SetJustification(ETextJustify::Left);
	InstructionsText->SetAutoWrapText(true);
	VBox->AddChild(InstructionsText);

	StartButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("StartButton"));
	VBox->AddChild(StartButton);

	UTextBlock* StartTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StartTxt"));
	StartTxt->SetText(FText::FromString(TEXT("START GAME")));
	StartTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	StartTxt->SetJustification(ETextJustify::Center);
	StartButton->SetContent(StartTxt);

	StartButton->OnClicked.AddDynamic(this, &UZTDInstructionsWidget::OnStartClicked);
}

void UZTDInstructionsWidget::OnStartClicked()
{
	AZTDPlayerController* PC = Cast<AZTDPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->HideInstructions();

		AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM)
		{
			// Just dismiss instructions - HUD will show wave screen based on game state
		}
	}
}

void UZTDInstructionsWidget::OnSkipClicked()
{
	AZTDPlayerController* PC = Cast<AZTDPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->HideInstructions();

		AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM)
		{
			// Just dismiss instructions - HUD will show wave screen based on game state
		}
	}
}

void UZTDInstructionsWidget::UpdateInstructions(const FString& NewTitle, const FString& NewInstructions)
{
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(NewTitle));
	}
	if (InstructionsText)
	{
		InstructionsText->SetText(FText::FromString(NewInstructions));
	}
}
