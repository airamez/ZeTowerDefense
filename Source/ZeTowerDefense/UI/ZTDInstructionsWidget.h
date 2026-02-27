#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZTDInstructionsWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class ZETOWERDEFENSE_API UZTDInstructionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Instructions")
	void UpdateInstructions(const FString& Title, const FString& Instructions);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Instructions")
	void OnStartClicked();

	UFUNCTION()
	void OnSkipClicked();

	UTextBlock* TitleText = nullptr;
	UTextBlock* InstructionsText = nullptr;
	UButton* StartButton = nullptr;
	UButton* SkipButton = nullptr;
};
