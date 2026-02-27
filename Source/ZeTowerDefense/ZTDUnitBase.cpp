#include "ZTDUnitBase.h"
#include "ZTDProjectile.h"
#include "ZTDDefenderUnit.h"
#include "Kismet/GameplayStatics.h"

AZTDUnitBase::AZTDUnitBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create capsule as root for better collision
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetCapsuleSize(50.0f, 100.0f); // Default size
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionObjectType(ECC_Pawn);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CapsuleComponent->SetGenerateOverlapEvents(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SceneRoot->SetupAttachment(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Let capsule handle collision
}

void AZTDUnitBase::BeginPlay()
{
	Super::BeginPlay();
	// CurrentHP is now the only HP value, initialized from default value
}

void AZTDUnitBase::FaceDirection(const FVector& Direction, float DeltaTime)
{
	if (Direction.IsNearlyZero()) return;
	FRotator TargetRot = Direction.Rotation();
	FRotator Combined(
		TargetRot.Pitch + MeshRotationOffset.Pitch,
		TargetRot.Yaw   + MeshRotationOffset.Yaw,
		TargetRot.Roll  + MeshRotationOffset.Roll);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), Combined, DeltaTime, TurnRate));
}

void AZTDUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead) return;

	FireCooldownTimer -= DeltaTime;

	FindTarget();

	if (CurrentTarget && FireCooldownTimer <= 0.0f)
	{
		FireAtTarget(CurrentTarget);
		FireCooldownTimer = 1.0f / FMath::Max(FireRate, 0.01f);
	}
}

void AZTDUnitBase::TakeDamageAmount(float DamageAmount)
{
	if (bIsDead) return;

	CurrentHP -= DamageAmount;
	if (CurrentHP <= 0.0f)
	{
		CurrentHP = 0.0f;
		HandleDeath();
	}
}

bool AZTDUnitBase::IsAlive() const
{
	return !bIsDead && CurrentHP > 0.0f;
}

void AZTDUnitBase::FireAtTarget(AActor* Target)
{
	if (!Target) return;

	// Check if this is a placement preview - placement previews should not fire
	AZTDDefenderUnit* Defender = Cast<AZTDDefenderUnit>(this);
	if (Defender && Defender->bIsPlacementPreview)
	{
		return;
	}

	// Rotate toward target
	FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FaceDirection(Direction, GetWorld()->GetDeltaSeconds());

	if (ProjectileClass)
	{
		// Spawn projectile at barrel offset position
		FVector SpawnLoc = GetActorLocation() + GetActorRotation().RotateVector(ProjectileSpawnOffset);
		FRotator SpawnRot = Direction.IsNearlyZero() ? GetActorRotation() : Direction.Rotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Owner = this;

		AZTDProjectile* Projectile = GetWorld()->SpawnActor<AZTDProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
		if (Projectile)
		{
			Projectile->SetActorScale3D(ProjectileScale);
			Projectile->Initialize(Target, Power, ProjectileSpeed);
		}
	}
	else
	{
		// Fallback: instant damage if no projectile class assigned
		AZTDUnitBase* TargetUnit = Cast<AZTDUnitBase>(Target);
		if (TargetUnit)
		{
			TargetUnit->TakeDamageAmount(Power);
		}
	}
}

bool AZTDUnitBase::CanAttackUnit(const AZTDUnitBase* Other) const
{
	if (!Other) return false;
	// Heli can attack both Heli and Tank
	// Tank can only attack Tank
	if (UnitType == EZTDUnitType::Heli) return true;
	return Other->UnitType == EZTDUnitType::Tank;
}

void AZTDUnitBase::HandleDeath()
{
	bIsDead = true;
	OnUnitDestroyed.Broadcast(this);

	// Spawn explosion effect if configured
	if (ExplosionConfig.NiagaraSystem || ExplosionConfig.ParticleSystem || ExplosionConfig.ExplosionSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning explosion for unit: %s"), *GetName());
		AZTDExplosionEffect::SpawnExplosion(this, GetActorLocation(), ExplosionConfig);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No explosion effect configured on unit: %s"), *GetName());
	}

	// Delay destroy slightly to allow effects to be visible
	SetLifeSpan(0.1f);
}

void AZTDUnitBase::FindTarget()
{
	// Override in subclasses
}

void AZTDUnitBase::TestNiagaraSystems()
{
	UE_LOG(LogTemp, Warning, TEXT("Testing Niagara system loading..."));
	
	// Try to load a specific explosion asset
	UNiagaraSystem* TestSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/VDBPack_01/VDB_Assets/Explosion_01_LOD_2"));
	if (TestSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded Explosion_01_LOD_2: %s"), *TestSystem->GetName());
		ExplosionConfig.NiagaraSystem = TestSystem; // Auto-assign for testing
		ExplosionConfig.ExplosionScale = 1.0f;
		ExplosionConfig.LifeSpan = 3.0f;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Explosion_01_LOD_2"));
	}
}
