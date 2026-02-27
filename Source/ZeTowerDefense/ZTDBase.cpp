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
	CurrentHP = MaxHP;
	OnBaseHPChanged.Broadcast(CurrentHP, MaxHP);
}

void AZTDBase::TakeDamageAmount(float DamageAmount)
{
	if (bIsDestroyed) return;

	CurrentHP = FMath::Max(CurrentHP - DamageAmount, 0.0f);
	OnBaseHPChanged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0.0f)
	{
		bIsDestroyed = true;
		OnBaseDestroyed.Broadcast();
	}
}

float AZTDBase::GetHPPercent() const
{
	return MaxHP > 0.0f ? CurrentHP / MaxHP : 0.0f;
}
