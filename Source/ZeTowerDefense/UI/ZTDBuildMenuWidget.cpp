#include "ZTDBuildMenuWidget.h"
#include "../ZTDPlayerController.h"
#include "../ZTDGameMode.h"
#include "../ZTDWaveSpawner.h"
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
	Border->SetPadding(FMargin(5.0f)); // Reduced padding
	Canvas->AddChild(Border);
	if (UCanvasPanelSlot* PSlot = Cast<UCanvasPanelSlot>(Border->Slot))
	{
		PSlot->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
		PSlot->SetAlignment(FVector2D(0.0f, 1.0f));
		PSlot->SetSize(FVector2D(275.0f, 350.0f)); // Increased height to ensure buttons fit
		PSlot->SetPosition(FVector2D(20.0f, -20.0f));
	}

	// Vertical box inside border
	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	// "Info" section header
	UTextBlock* InfoHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InfoHeader"));
	InfoHeader->SetText(FText::FromString(TEXT("Info")));
	InfoHeader->SetJustification(ETextJustify::Center);
	InfoHeader->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(InfoHeader);
	if (UVerticalBoxSlot* IHSlot = Cast<UVerticalBoxSlot>(InfoHeader->Slot))
	{
		IHSlot->SetPadding(FMargin(2.0f, 1.0f, 2.0f, 2.0f)); // Reduced padding
	}

	// Info content
	AZTDGameMode* GM = GetWorld() ? Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld())) : nullptr;
	if (GM)
	{
		// Base HP
		UTextBlock* BaseHPText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BaseHPText"));
		float BaseHP = GM->GetBaseHP();
		FLinearColor HPColor = (BaseHP < 25.0f) ? FLinearColor::Red : FLinearColor::White;
		BaseHPText->SetText(FText::FromString(FString::Printf(TEXT("Base HP: %.0f"), BaseHP)));
		BaseHPText->SetColorAndOpacity(FSlateColor(HPColor));
		BaseHPText->SetJustification(ETextJustify::Center);
		VBox->AddChild(BaseHPText);
		if (UVerticalBoxSlot* BPSlot = Cast<UVerticalBoxSlot>(BaseHPText->Slot))
		{
			BPSlot->SetPadding(FMargin(2.0f, 0.5f, 2.0f, 0.5f)); // Reduced padding
		}

		// Points
		UTextBlock* PointsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PointsText"));
		PointsText->SetText(FText::FromString(FString::Printf(TEXT("Points: %d"), GM->GetPlayerPoints())));
		PointsText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		PointsText->SetJustification(ETextJustify::Center);
		VBox->AddChild(PointsText);
		if (UVerticalBoxSlot* PSlot = Cast<UVerticalBoxSlot>(PointsText->Slot))
		{
			PSlot->SetPadding(FMargin(2.0f, 0.5f, 2.0f, 0.5f)); // Reduced padding
		}

		// Wave
		UTextBlock* WaveText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("WaveText"));
		WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), GM->CurrentWaveNumber)));
		WaveText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		WaveText->SetJustification(ETextJustify::Center);
		VBox->AddChild(WaveText);
		if (UVerticalBoxSlot* WSlot = Cast<UVerticalBoxSlot>(WaveText->Slot))
		{
			WSlot->SetPadding(FMargin(2.0f, 0.5f, 2.0f, 0.5f)); // Reduced padding
		}

		// Tank counter
		UTextBlock* TankText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TankText"));
		if (GM->WaveSpawner)
		{
			int32 TotalTanks = GM->WaveSpawner->GetTankCountForWave(GM->CurrentWaveNumber);
			int32 TanksKilled = GM->WaveSpawner->GetTanksKilledThisWave();
			TankText->SetText(FText::FromString(FString::Printf(TEXT("Tank: %d/%d"), TanksKilled, TotalTanks)));
		}
		else
		{
			TankText->SetText(FText::FromString(TEXT("Tank: 0/0")));
		}
		TankText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TankText->SetJustification(ETextJustify::Center);
		VBox->AddChild(TankText);
		if (UVerticalBoxSlot* TSlot = Cast<UVerticalBoxSlot>(TankText->Slot))
		{
			TSlot->SetPadding(FMargin(2.0f, 0.5f, 2.0f, 2.0f)); // Reduced padding before Build section
		}
	}

	// Separator line
	UTextBlock* Separator = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Separator"));
	Separator->SetText(FText::FromString(TEXT("---")));
	Separator->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f)));
	Separator->SetJustification(ETextJustify::Center);
	VBox->AddChild(Separator);
	if (UVerticalBoxSlot* SSlot = Cast<UVerticalBoxSlot>(Separator->Slot))
	{
		SSlot->SetPadding(FMargin(2.0f, 1.0f, 2.0f, 2.0f)); // Reduced padding
	}

	// Helper lambda-style: create a button with text inside VBox
	auto MakeButton = [&](const FString& Name, const FString& Label) -> UButton*
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *Name);
		VBox->AddChild(Btn);
		if (UVerticalBoxSlot* BSlot = Cast<UVerticalBoxSlot>(Btn->Slot))
		{
			BSlot->SetPadding(FMargin(1.0f)); // Reduced padding
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
		HSlot->SetPadding(FMargin(2.0f, 1.0f, 2.0f, 2.0f)); // Reduced padding
	}

	// Build Tank button
	BuildTankButton = MakeButton(TEXT("BuildTank"), TEXT("Tank = 5 points"));
	TankCostText = Cast<UTextBlock>(BuildTankButton->GetChildAt(0));

	// Build Heli button
	BuildHeliButton = MakeButton(TEXT("BuildHeli"), TEXT("Heli = 15 points"));
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
			TankCostText->SetText(FText::FromString(FString::Printf(TEXT("Tank = %d points"), TankCost)));
		}
		
		// Get Heli cost
		if (HeliCostText && PC->DefenderHeliClass)
		{
			AZTDDefenderUnit* DefaultHeli = PC->DefenderHeliClass->GetDefaultObject<AZTDDefenderUnit>();
			int32 HeliCost = DefaultHeli ? DefaultHeli->SpawnCost : 15;
			HeliCostText->SetText(FText::FromString(FString::Printf(TEXT("Heli = %d points"), HeliCost)));
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

