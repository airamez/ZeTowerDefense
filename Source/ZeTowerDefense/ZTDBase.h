#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZTDBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBaseDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBaseHPChanged, float, CurrentHP, float, MaxHP);

UCLASS(Blueprintable)
class ZETOWERDEFENSE_API AZTDBase : public AActor
{
	GENERATED_BODY()

public:
	AZTDBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHP = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBaseDestroyed OnBaseDestroyed;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBaseHPChanged OnBaseHPChanged;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TakeDamageAmount(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetHPPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsDestroyed() const { return bIsDestroyed; }

protected:
	virtual void BeginPlay() override;

	bool bIsDestroyed = false;
};
