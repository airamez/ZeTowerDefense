#include "ZTDUnitMenuWidget.h"
#include "../ZTDDefenderUnit.h"
#include "../ZTDGameMode.h"
#include "../ZTDPlayerController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void UZTDUnitMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree) return;

	// Root canvas
	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	// Background border centered (same as build menu)
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.85f));
	Border->SetPadding(FMargin(10.0f));
	Canvas->AddChild(Border);
	if (UCanvasPanelSlot* PSlot = Cast<UCanvasPanelSlot>(Border->Slot))
	{
		PSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f)); // Centered like build menu
		PSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		PSlot->SetSize(FVector2D(450.0f, 400.0f)); // Wider for stat + button layout
		PSlot->SetPosition(FVector2D(0.0f, 0.0f));
	}

	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	// Helper lambda-style: create a button with text inside VBox (same as build menu)
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

	// Helper lambda-style: create a stat row with label and upgrade button on same line
	auto MakeStatRow = [&](const FString& StatName, UTextBlock*& StatText, UTextBlock*& CostText, UButton*& UpgradeBtn)
	{
		// Stat row container
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), *(StatName + TEXT("Row")));
		VBox->AddChild(Row);
		if (UVerticalBoxSlot* RSlot = Cast<UVerticalBoxSlot>(Row->Slot))
		{
			RSlot->SetPadding(FMargin(0.0f, 2.0f));
		}

		// Stat text (left side)
		StatText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(StatName + TEXT("Text")));
		StatText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Row->AddChild(StatText);
		if (UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(StatText->Slot))
		{
			HSlot->SetSize(ESlateSizeRule::Fill); // Take available space
			HSlot->SetHorizontalAlignment(HAlign_Left);
		}

		// Upgrade button (right side)
		UpgradeBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *(StatName + TEXT("Btn")));
		Row->AddChild(UpgradeBtn);
		if (UHorizontalBoxSlot* BtnSlot = Cast<UHorizontalBoxSlot>(UpgradeBtn->Slot))
		{
			BtnSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic)); // Auto size based on content
		}

		// Cost text inside button
		CostText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(StatName + TEXT("Cost")));
		CostText->SetJustification(ETextJustify::Center);
		CostText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UpgradeBtn->AddChild(CostText);
	};

	// Unit name
	UnitNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("UnitName"));
	UnitNameText->SetText(FText::FromString(TEXT("Unit")));
	UnitNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	UnitNameText->SetJustification(ETextJustify::Center);
	VBox->AddChild(UnitNameText);

	// Stat rows with upgrade buttons (no Speed upgrade for static defenders)
	MakeStatRow(TEXT("HP"), HPText, HPCostText, UpgradeHPButton);
	MakeStatRow(TEXT("FireRate"), FireRateText, FireRateCostText, UpgradeFireRateButton);
	MakeStatRow(TEXT("Power"), PowerText, PowerCostText, UpgradePowerButton);
	MakeStatRow(TEXT("Range"), RangeText, RangeCostText, UpgradeRangeButton);

	// Close button
	CloseButton = MakeButton(TEXT("Close"), TEXT("Close"));

	// Bind events (no Speed upgrade for static defenders)
	UpgradeHPButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnUpgradeHPClicked);
	UpgradeFireRateButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnUpgradeFireRateClicked);
	UpgradePowerButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnUpgradePowerClicked);
	UpgradeRangeButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnUpgradeRangeClicked);
	CloseButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnCloseClicked);
}

void UZTDUnitMenuWidget::SetUnit(AZTDDefenderUnit* Unit)
{
	TargetUnit = Unit;
	RefreshStats();
}

void UZTDUnitMenuWidget::RefreshStats()
{
	if (!TargetUnit) return;

	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	int32 PlayerPoints = GM ? GM->PlayerPoints : 0;

	if (UnitNameText)
	{
		// Extract clean unit name from blueprint class name
		FString UnitName = TargetUnit->GetName();
		if (UnitName.Contains(TEXT("Heli")))
		{
			UnitName = TEXT("Helicopter");
		}
		else if (UnitName.Contains(TEXT("Tank")))
		{
			UnitName = TEXT("Tank");
		}
		UnitNameText->SetText(FText::FromString(UnitName));
	}

	if (FireRateText)
	{
		FireRateText->SetText(FText::FromString(FString::Printf(TEXT("Fire Rate: %.2f (Lv %d)"), TargetUnit->FireRate, TargetUnit->FireRateLevel)));
	}

	if (PowerText)
	{
		PowerText->SetText(FText::FromString(FString::Printf(TEXT("Power: %.1f (Lv %d)"), TargetUnit->Power, TargetUnit->PowerLevel)));
	}

	if (RangeText)
	{
		RangeText->SetText(FText::FromString(FString::Printf(TEXT("Range: %.0f (Lv %d)"), TargetUnit->AttackRange, TargetUnit->RangeLevel)));
	}

	if (HPText)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("HP: %.0f"), TargetUnit->CurrentHP)));
	}

	int32 HPCost = TargetUnit->GetHPUpgradeCost();
	int32 FireRateCost = TargetUnit->GetFireRateUpgradeCost();
	int32 PowerCost = TargetUnit->GetPowerUpgradeCost();
	int32 RangeCost = TargetUnit->GetRangeUpgradeCost();

	if (HPCostText)
	{
		HPCostText->SetText(FText::FromString(FString::Printf(TEXT("Upgrade Cost = %d"), HPCost)));
	}
	if (FireRateCostText)
	{
		FireRateCostText->SetText(FText::FromString(FString::Printf(TEXT("Upgrade Cost = %d"), FireRateCost)));
	}
	if (PowerCostText)
	{
		PowerCostText->SetText(FText::FromString(FString::Printf(TEXT("Upgrade Cost = %d"), PowerCost)));
	}
	if (RangeCostText)
	{
		RangeCostText->SetText(FText::FromString(FString::Printf(TEXT("Upgrade Cost = %d"), RangeCost)));
	}

	// Enable/disable buttons based on available points (no Speed upgrade for static defenders)
	if (UpgradeHPButton)
	{
		UpgradeHPButton->SetIsEnabled(PlayerPoints >= HPCost);
	}
	if (UpgradeFireRateButton)
	{
		UpgradeFireRateButton->SetIsEnabled(PlayerPoints >= FireRateCost);
	}
	if (UpgradePowerButton)
	{
		UpgradePowerButton->SetIsEnabled(PlayerPoints >= PowerCost);
	}
	if (UpgradeRangeButton)
	{
		UpgradeRangeButton->SetIsEnabled(PlayerPoints >= RangeCost);
	}
	if (UpgradeHPButton)
	{
		UpgradeHPButton->SetIsEnabled(PlayerPoints >= HPCost);
	}
}

void UZTDUnitMenuWidget::OnUpgradeFireRateClicked()
{
	if (!TargetUnit) return;

	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	if (TargetUnit->UpgradeFireRate(GM->PlayerPoints))
	{
		GM->OnPointsChanged.Broadcast(GM->PlayerPoints);
		RefreshStats();
	}
}

void UZTDUnitMenuWidget::OnUpgradePowerClicked()
{
	if (!TargetUnit) return;

	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	if (TargetUnit->UpgradePower(GM->PlayerPoints))
	{
		GM->OnPointsChanged.Broadcast(GM->PlayerPoints);
		RefreshStats();
	}
}

void UZTDUnitMenuWidget::OnUpgradeHPClicked()
{
	if (!TargetUnit) return;

	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	if (TargetUnit->UpgradeHP(GM->PlayerPoints))
	{
		GM->OnPointsChanged.Broadcast(GM->PlayerPoints);
		RefreshStats();
	}
}

void UZTDUnitMenuWidget::OnUpgradeRangeClicked()
{
	if (!TargetUnit) return;

	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	if (TargetUnit->UpgradeRange(GM->PlayerPoints))
	{
		RefreshStats();
	}
}

void UZTDUnitMenuWidget::OnCloseClicked()
{
	AZTDPlayerController* PC = Cast<AZTDPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->HideUnitMenu();
	}
}
