#include "ZTDBuildMenuWidget.h"
#include "../ZTDPlayerController.h"
#include "../ZTDGameMode.h"
#include "../ZTDDefenderUnit.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void UZTDBuildMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree) 
	{
		return;
	}

	// Root canvas
	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;
	
	// Background border positioned in bottom-left corner
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.85f));
	Border->SetPadding(FMargin(10.0f));
	Canvas->AddChild(Border);
	if (UCanvasPanelSlot* PSlot = Cast<UCanvasPanelSlot>(Border->Slot))
	{
		PSlot->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
		PSlot->SetAlignment(FVector2D(0.0f, 1.0f));
		PSlot->SetSize(FVector2D(400.0f, 180.0f)); // Doubled width from 200 to 400
		PSlot->SetPosition(FVector2D(20.0f, -20.0f));
	}

	// Vertical box inside border
	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	// Helper lambda-style: create a button with text inside VBox
	auto MakeButton = [&](const FString& Name, const FString& Label) -> UButton*
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *Name);
		VBox->AddChild(Btn);
		if (UVerticalBoxSlot* BSlot = Cast<UVerticalBoxSlot>(Btn->Slot))
		{
			BSlot->SetPadding(FMargin(2.0f));
			// Set fixed width for all buttons
			BSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}
		UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(Name + TEXT("_Text")));
		Txt->SetText(FText::FromString(Label));
		Txt->SetJustification(ETextJustify::Center);
		Btn->AddChild(Txt);
		return Btn;
	};

	// "Build" header text
	UTextBlock* HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeaderText"));
	HeaderText->SetText(FText::FromString(TEXT("Build")));
	HeaderText->SetJustification(ETextJustify::Center);
	HeaderText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(HeaderText);
	if (UVerticalBoxSlot* HSlot = Cast<UVerticalBoxSlot>(HeaderText->Slot))
	{
		HSlot->SetPadding(FMargin(2.0f, 2.0f, 2.0f, 6.0f));
	}

	// Build Tank button
	BuildTankButton = MakeButton(TEXT("BuildTank"), TEXT("Build Tank (5 pts)"));
	TankCostText = Cast<UTextBlock>(BuildTankButton->GetChildAt(0));

	// Build Heli button
	BuildHeliButton = MakeButton(TEXT("BuildHeli"), TEXT("Build Heli (5 pts)"));
	HeliCostText = Cast<UTextBlock>(BuildHeliButton->GetChildAt(0));

	// Bind button events
	BuildTankButton->OnClicked.AddDynamic(this, &UZTDBuildMenuWidget::OnBuildTankClicked);
	BuildHeliButton->OnClicked.AddDynamic(this, &UZTDBuildMenuWidget::OnBuildHeliClicked);

	// Update costs from defender classes
	AZTDPlayerController* PC = Cast<AZTDPlayerController>(GetOwningPlayer());
	if (PC)
	{
		// Get Tank cost
		if (TankCostText && PC->DefenderTankClass)
		{
			AZTDDefenderUnit* DefaultTank = PC->DefenderTankClass->GetDefaultObject<AZTDDefenderUnit>();
			int32 TankCost = DefaultTank ? DefaultTank->SpawnCost : 5;
			TankCostText->SetText(FText::FromString(FString::Printf(TEXT("Build Tank (%d pts)"), TankCost)));
		}
		
		// Get Heli cost
		if (HeliCostText && PC->DefenderHeliClass)
		{
			AZTDDefenderUnit* DefaultHeli = PC->DefenderHeliClass->GetDefaultObject<AZTDDefenderUnit>();
			int32 HeliCost = DefaultHeli ? DefaultHeli->SpawnCost : 5;
			HeliCostText->SetText(FText::FromString(FString::Printf(TEXT("Build Heli (%d pts)"), HeliCost)));
		}
	}
	
	// Force widget to be visible at the end of construction
	SetVisibility(ESlateVisibility::Visible);
}

void UZTDBuildMenuWidget::OnBuildTankClicked()
{
	AZTDPlayerController* PC = Cast<AZTDPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->SelectBuildType(EZTDBuildType::Tank);
	}
}

void UZTDBuildMenuWidget::OnBuildHeliClicked()
{
	AZTDPlayerController* PC = Cast<AZTDPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->SelectBuildType(EZTDBuildType::Heli);
	}
}

void UZTDBuildMenuWidget::OnCancelClicked()
{
	// No longer used - build menu is always visible
}

