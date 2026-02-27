#include "ZTDEnemyUnit.h"
#include "ZTDBase.h"
#include "ZTDDefenderUnit.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"

AZTDEnemyUnit::AZTDEnemyUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	// Add movement component for better ground following
	MovementComponent = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("MovementComponent"));
	if (MovementComponent)
	{
		UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(MovementComponent);
		if (CharMovement)
		{
			CharMovement->bOrientRotationToMovement = false;
			CharMovement->bUseControllerDesiredRotation = false;
			CharMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
			CharMovement->GravityScale = 1.0f;
			CharMovement->AirControl = 0.2f;
			CharMovement->GroundFriction = 3.0f;
			CharMovement->SetComponentTickEnabled(false); // Disable tick to prevent conflicts
			CharMovement->SetMovementMode(MOVE_None); // Disable movement
		}
	}
}

void AZTDEnemyUnit::BeginPlay()
{
	Super::BeginPlay();

	// Find the base in the world
	TArray<AActor*> FoundBases;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZTDBase::StaticClass(), FoundBases);
	if (FoundBases.Num() > 0)
	{
		TargetBase = Cast<AZTDBase>(FoundBases[0]);
	}
}

void AZTDEnemyUnit::InitializeStats(float InSpeed, float InFireRate, float InHP, float InPower, float InAttackRange)
{
	Speed = InSpeed;
	FireRate = InFireRate;
	CurrentHP = InHP; // Only use CurrentHP now
	Power = InPower;
	AttackRange = InAttackRange;
}

void AZTDEnemyUnit::Tick(float DeltaTime)
{
	if (bIsDead) return;

	FireCooldownTimer -= DeltaTime;

	// Only find a new target if we don't have one or our current target is dead
	if (!CurrentTarget || !Cast<AZTDUnitBase>(CurrentTarget)->IsAlive())
	{
		FindTarget();
	}

	if (CurrentTarget)
	{
		// In combat range of a defender - face and fire
		FVector Direction = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FaceDirection(Direction, DeltaTime);

		if (FireCooldownTimer <= 0.0f)
		{
			FireAtTarget(CurrentTarget);
			FireCooldownTimer = 1.0f / FMath::Max(FireRate, 0.01f);
		}
	}
	else if (AggroTarget && IsValid(AggroTarget) && AggroTarget->IsAlive())
	{
		// Diverted by a defender that fired at us - move toward it and stop.
		// Even if we can't fire back (e.g. Tank diverted by Heli), we stop advancing
		// toward the base and face the attacker until it's destroyed.
		float DistToAggro = FVector::Dist(GetActorLocation(), AggroTarget->GetActorLocation());
		if (DistToAggro > AttackRange)
		{
			MoveTowardTarget(AggroTarget, DeltaTime);
		}
		else
		{
			// In range - face the attacker and stay put
			FVector Direction = (AggroTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			FaceDirection(Direction, DeltaTime);
			// Only set as CurrentTarget if we can actually fire back at it
			if (CanAttackUnit(AggroTarget))
			{
				CurrentTarget = AggroTarget;
			}
		}
	}
	else if (TargetBase && !TargetBase->IsDestroyed())
	{
		AggroTarget = nullptr; // No aggro, reset
		float DistToBase = FVector::Dist(GetActorLocation(), TargetBase->GetActorLocation());

		if (DistToBase > AttackDistanceToBase)
		{
			MoveTowardBase(DeltaTime);
		}
		else
		{
			// In range of base, face and fire
			FVector Direction = (TargetBase->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			if (!Direction.IsNearlyZero())
			{
				FaceDirection(Direction, DeltaTime);
			}

			if (FireCooldownTimer <= 0.0f)
			{
				// Fire at base - always instant damage (no projectile target needed)
				TargetBase->TakeDamageAmount(BaseDamage);
				FireCooldownTimer = 1.0f / FMath::Max(FireRate, 0.01f);
			}
		}
	}
}

void AZTDEnemyUnit::FindTarget()
{
	// Look for closest compatible defending unit in attack range
	AZTDDefenderUnit* ClosestDefender = FindClosestDefender();
	CurrentTarget = ClosestDefender;
	
	// Debug: Check all tanks to see if any are targeting placement previews
	if (GetName().Contains(TEXT("Tank_")) && CurrentTarget)
	{
		AZTDDefenderUnit* DefenderTarget = Cast<AZTDDefenderUnit>(CurrentTarget);
		if (DefenderTarget && DefenderTarget->bIsPlacementPreview)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is targeting PLACEMENT PREVIEW: %s"), 
				*GetName(), *CurrentTarget->GetName());
		}
	}
}

void AZTDEnemyUnit::NotifyAttackedBy(AZTDDefenderUnit* Attacker)
{
	if (!Attacker || !Attacker->IsAlive()) return;
	// Divert toward attacker regardless of whether we can fire back.
	// A tank being shot by a heli will stop advancing and move toward the heli,
	// even though it cannot return fire (tank vs heli targeting rule).
	if (!CurrentTarget)
	{
		AggroTarget = Attacker;
	}
}

void AZTDEnemyUnit::MoveTowardBase(float DeltaTime)
{
	if (!TargetBase) return;

	// Debug logging
	UE_LOG(LogTemp, Warning, TEXT("Enemy %s moving toward base with Speed: %.1f"), *GetName(), Speed);

	FVector Direction = (TargetBase->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector NewLocation = GetActorLocation() + Direction * Speed * DeltaTime;

	// Debug logging
	UE_LOG(LogTemp, Warning, TEXT("Enemy %s: CurrentPos: %s, NewPos: %s, DeltaTime: %.3f"), 
		*GetName(), *GetActorLocation().ToString(), *NewLocation.ToString(), DeltaTime);

	// Ground detection - trace down from proposed position
	FHitResult GroundHit;
	FVector TraceStart = NewLocation + FVector(0, 0, 500.0f); // Start above ground
	FVector TraceEnd = NewLocation - FVector(0, 0, 2000.0f); // Trace down further

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	// Try multiple collision channels for better ground detection
	bool bHitGround = GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams);
	if (!bHitGround)
	{
		// Try with WorldDynamic channel
		bHitGround = GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldDynamic, QueryParams);
	}

	if (bHitGround)
	{
		// Place enemy on ground with proper height
		float CapsuleHalfHeight = 50.0f; // Half of capsule height
		NewLocation.Z = GroundHit.Location.Z + CapsuleHalfHeight + 10.0f;
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		// No ground detected, use the calculated position with default height
		NewLocation.Z = 100.0f; // Default height if no ground detected
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	// Face movement direction
	FaceDirection(Direction, DeltaTime);
}

AZTDDefenderUnit* AZTDEnemyUnit::FindClosestDefender() const
{
	AZTDDefenderUnit* Closest = nullptr;
	float ClosestDist = AttackRange;

	for (TActorIterator<AZTDDefenderUnit> It(GetWorld()); It; ++It)
	{
		AZTDDefenderUnit* Defender = *It;
		// Skip placement previews - they're not real units
		if (Defender && Defender->IsAlive() && !Defender->bIsPlacementPreview && CanAttackUnit(Defender))
		{
			float Dist = FVector::Dist(GetActorLocation(), Defender->GetActorLocation());
			if (Dist < ClosestDist)
			{
				ClosestDist = Dist;
				Closest = Defender;
			}
		}
	}

	return Closest;
}

void AZTDEnemyUnit::MoveTowardTarget(AActor* Target, float DeltaTime)
{
	if (!Target) return;

	FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector NewLocation = GetActorLocation() + Direction * Speed * DeltaTime;

	// Ground detection
	FHitResult GroundHit;
	FVector TraceStart = NewLocation + FVector(0, 0, 500.0f);
	FVector TraceEnd = NewLocation - FVector(0, 0, 2000.0f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
	{
		NewLocation.Z = GroundHit.Location.Z + 60.0f;
	}

	SetActorLocation(NewLocation);
	FaceDirection(Direction, DeltaTime);
}

// HandleDeath now implemented in base class with explosion support
