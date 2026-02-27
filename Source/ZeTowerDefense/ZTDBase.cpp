#include "ZTDBase.h"

AZTDBase::AZTDBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
}

void AZTDBase::BeginPlay()
{
	Super::BeginPlay();
	// CurrentHP is now the only HP value, initialized from default value
	OnBaseHPChanged.Broadcast(CurrentHP);
}

void AZTDBase::TakeDamageAmount(float DamageAmount)
{
	if (bIsDestroyed) return;

	CurrentHP = FMath::Max(CurrentHP - DamageAmount, 0.0f);
	OnBaseHPChanged.Broadcast(CurrentHP);

	if (CurrentHP <= 0.0f)
	{
		bIsDestroyed = true;
		OnBaseDestroyed.Broadcast();
	}
}

float AZTDBase::GetHPPercent() const
{
	// Since we only have CurrentHP now, return 1.0f (full health) as base reference
	// This function might need redesign based on new HP system
	return CurrentHP > 0.0f ? 1.0f : 0.0f;
}
