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

	// Background border on right side
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.85f));
	Border->SetPadding(FMargin(10.0f));
	Canvas->AddChild(Border);
	if (UCanvasPanelSlot* PSlot = Cast<UCanvasPanelSlot>(Border->Slot))
	{
		PSlot->SetAnchors(FAnchors(1.0f, 0.5f, 1.0f, 0.5f));
		PSlot->SetAlignment(FVector2D(1.0f, 0.5f));
		PSlot->SetSize(FVector2D(280.0f, 350.0f));
		PSlot->SetPosition(FVector2D(-10.0f, 0.0f));
	}

	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
	Border->SetContent(VBox);

	// Unit name
	UnitNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("UnitName"));
	UnitNameText->SetText(FText::FromString(TEXT("Unit")));
	UnitNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	VBox->AddChild(UnitNameText);

	// HP text
	HPText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HP"));
	HPText->SetText(FText::FromString(TEXT("HP: --")));
	HPText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	VBox->AddChild(HPText);

	// Helper: create stat row with text + cost + upgrade button
	auto MakeStatRow = [&](const FString& Name, UTextBlock*& StatText, UTextBlock*& CostText, UButton*& UpgradeBtn)
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), *(Name + TEXT("Row")));
		VBox->AddChild(Row);
		if (UVerticalBoxSlot* RSlot = Cast<UVerticalBoxSlot>(Row->Slot))
		{
			RSlot->SetPadding(FMargin(0.0f, 2.0f));
		}

		StatText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(Name + TEXT("Stat")));
		StatText->SetText(FText::FromString(Name + TEXT(": --")));
		StatText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Row->AddChild(StatText);
		if (UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(StatText->Slot))
		{
			HSlot->SetSize(ESlateSizeRule::Fill);
		}

		UpgradeBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *(Name + TEXT("Btn")));
		Row->AddChild(UpgradeBtn);

		CostText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(Name + TEXT("Cost")));
		CostText->SetText(FText::FromString(TEXT("Up (0)")));
		CostText->SetJustification(ETextJustify::Center);
		UpgradeBtn->AddChild(CostText);
	};

	MakeStatRow(TEXT("Speed"), SpeedText, SpeedCostText, UpgradeSpeedButton);
	MakeStatRow(TEXT("FireRate"), FireRateText, FireRateCostText, UpgradeFireRateButton);
	MakeStatRow(TEXT("Power"), PowerText, PowerCostText, UpgradePowerButton);
	
	// HP upgrade row (no stat text display, just upgrade button)
	UHorizontalBox* HPRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HPRow"));
	VBox->AddChild(HPRow);
	if (UVerticalBoxSlot* RSlot = Cast<UVerticalBoxSlot>(HPRow->Slot))
	{
		RSlot->SetPadding(FMargin(0.0f, 2.0f));
	}

	UTextBlock* HPLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HPLabel"));
	HPLabel->SetText(FText::FromString(TEXT("HP: --")));
	HPLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	HPRow->AddChild(HPLabel);
	if (UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(HPLabel->Slot))
	{
		HSlot->SetSize(ESlateSizeRule::Fill);
	}

	UpgradeHPButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("HPBtn"));
	HPRow->AddChild(UpgradeHPButton);

	HPCostText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HPCost"));
	HPCostText->SetText(FText::FromString(TEXT("Up (0)")));
	HPCostText->SetJustification(ETextJustify::Center);
	UpgradeHPButton->AddChild(HPCostText);

	// Close button
	CloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("CloseBtn"));
	VBox->AddChild(CloseButton);
	if (UVerticalBoxSlot* CSlot = Cast<UVerticalBoxSlot>(CloseButton->Slot))
	{
		CSlot->SetPadding(FMargin(0.0f, 8.0f, 0.0f, 0.0f));
	}
	UTextBlock* CloseTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CloseTxt"));
	CloseTxt->SetText(FText::FromString(TEXT("Close")));
	CloseTxt->SetJustification(ETextJustify::Center);
	CloseButton->AddChild(CloseTxt);

	// Bind events
	UpgradeSpeedButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnUpgradeSpeedClicked);
	UpgradeFireRateButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnUpgradeFireRateClicked);
	UpgradePowerButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnUpgradePowerClicked);
	UpgradeHPButton->OnClicked.AddDynamic(this, &UZTDUnitMenuWidget::OnUpgradeHPClicked);
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
		UnitNameText->SetText(FText::FromString(TargetUnit->GetName()));
	}

	if (SpeedText)
	{
		SpeedText->SetText(FText::FromString(FString::Printf(TEXT("Speed: %.1f (Lv %d)"), TargetUnit->Speed, TargetUnit->SpeedLevel)));
	}

	if (FireRateText)
	{
		FireRateText->SetText(FText::FromString(FString::Printf(TEXT("Fire Rate: %.2f (Lv %d)"), TargetUnit->FireRate, TargetUnit->FireRateLevel)));
	}

	if (PowerText)
	{
		PowerText->SetText(FText::FromString(FString::Printf(TEXT("Power: %.1f (Lv %d)"), TargetUnit->Power, TargetUnit->PowerLevel)));
	}

	if (HPText)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("HP: %.0f"), TargetUnit->CurrentHP)));
	}

	int32 SpeedCost = TargetUnit->GetSpeedUpgradeCost();
	int32 FireRateCost = TargetUnit->GetFireRateUpgradeCost();
	int32 PowerCost = TargetUnit->GetPowerUpgradeCost();
	int32 HPCost = TargetUnit->GetHPUpgradeCost();

	if (SpeedCostText)
	{
		SpeedCostText->SetText(FText::FromString(FString::Printf(TEXT("Cost: %d"), SpeedCost)));
	}
	if (FireRateCostText)
	{
		FireRateCostText->SetText(FText::FromString(FString::Printf(TEXT("Cost: %d"), FireRateCost)));
	}
	if (PowerCostText)
	{
		PowerCostText->SetText(FText::FromString(FString::Printf(TEXT("Cost: %d"), PowerCost)));
	}
	if (HPCostText)
	{
		HPCostText->SetText(FText::FromString(FString::Printf(TEXT("Cost: %d"), HPCost)));
	}

	// Enable/disable buttons based on available points
	if (UpgradeSpeedButton)
	{
		UpgradeSpeedButton->SetIsEnabled(PlayerPoints >= SpeedCost);
	}
	if (UpgradeFireRateButton)
	{
		UpgradeFireRateButton->SetIsEnabled(PlayerPoints >= FireRateCost);
	}
	if (UpgradePowerButton)
	{
		UpgradePowerButton->SetIsEnabled(PlayerPoints >= PowerCost);
	}
	if (UpgradeHPButton)
	{
		UpgradeHPButton->SetIsEnabled(PlayerPoints >= HPCost);
	}
}

void UZTDUnitMenuWidget::OnUpgradeSpeedClicked()
{
	if (!TargetUnit) return;

	AZTDGameMode* GM = Cast<AZTDGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	if (TargetUnit->UpgradeSpeed(GM->PlayerPoints))
	{
		GM->OnPointsChanged.Broadcast(GM->PlayerPoints);
		RefreshStats();
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

void UZTDUnitMenuWidget::OnCloseClicked()
{
	AZTDPlayerController* PC = Cast<AZTDPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->HideUnitMenu();
	}
}
