#include "ZTDProjectile.h"
#include "ZTDUnitBase.h"
#include "ZTDBase.h"

AZTDProjectile::AZTDProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AZTDProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AZTDProjectile::Initialize(AActor* InTarget, float InDamage, float InSpeed)
{
	Target = InTarget;
	Damage = InDamage;
	Speed = InSpeed;
}

void AZTDProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LifeTimer += DeltaTime;
	if (LifeTimer >= MaxLifeTime)
	{
		Destroy();
		return;
	}

	// If target is gone, keep flying forward and destroy
	if (!Target || !IsValid(Target))
	{
		FVector Forward = GetActorForwardVector();
		SetActorLocation(GetActorLocation() + Forward * Speed * DeltaTime);

		LifeTimer += DeltaTime * 5.0f; // expire faster if no target
		return;
	}

	// Move toward target
	FVector MyLoc = GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();
	FVector Direction = (TargetLoc - MyLoc);
	float DistToTarget = Direction.Size();
	Direction.Normalize();

	// Rotate to face target, applying projectile's own mesh offset
	if (!Direction.IsNearlyZero())
	{
		FRotator TargetRot = Direction.Rotation();
		FRotator OffsetTargetRot(
			TargetRot.Pitch + MeshRotationOffset.Pitch,
			TargetRot.Yaw   + MeshRotationOffset.Yaw,
			TargetRot.Roll  + MeshRotationOffset.Roll);
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), OffsetTargetRot, DeltaTime, 10.0f));
	}

	float MoveDistance = Speed * DeltaTime;

	if (DistToTarget <= MoveDistance)
	{
		// Reached target - apply damage and destroy
		AZTDUnitBase* UnitTarget = Cast<AZTDUnitBase>(Target);
		if (UnitTarget && UnitTarget->IsAlive())
		{
			UnitTarget->TakeDamageAmount(Damage);
		}
		else
		{
			AZTDBase* BaseTarget = Cast<AZTDBase>(Target);
			if (BaseTarget)
			{
				BaseTarget->TakeDamageAmount(Damage);
			}
		}
		Destroy();
	}
	else
	{
		SetActorLocation(MyLoc + Direction * MoveDistance);
	}
}
