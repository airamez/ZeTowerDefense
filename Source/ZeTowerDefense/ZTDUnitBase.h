#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "ZTDProjectile.h"
#include "ZTDExplosionEffect.h"
#include "ZTDExplosionTypes.h"
#include "ZTDUnitBase.generated.h"

UENUM(BlueprintType)
enum class EZTDUnitType : uint8
{
	Heli UMETA(DisplayName = "Helicopter"),
	Tank UMETA(DisplayName = "Tank")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDestroyed, AZTDUnitBase*, DestroyedUnit);

UCLASS(Abstract, Blueprintable)
class ZETOWERDEFENSE_API AZTDUnitBase : public AActor
{
	GENERATED_BODY()

public:
	AZTDUnitBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// --- Mesh Orientation Fix ---
	// Adjust this per Blueprint if the mesh moves sideways (e.g. 0,0,90 or 0,0,-90)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FRotator MeshRotationOffset = FRotator(0.0f, 0.0f, 0.0f);

	// How fast the unit rotates to face its target (degrees/sec interpolation speed)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float TurnRate = 15.0f;

	// --- Unit Type ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EZTDUnitType UnitType = EZTDUnitType::Tank;

	// --- Projectile ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AZTDProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FVector ProjectileScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileSpeed = 2000.0f;

	// --- Unit Stats ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Speed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float FireRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Power = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackRange = 1500.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnUnitDestroyed OnUnitDestroyed;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TakeDamageAmount(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireAtTarget(AActor* Target);

	// Returns true if this unit can attack the given unit type
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CanAttackUnit(const AZTDUnitBase* Other) const;

	// Spawn location offset for projectile (e.g. gun barrel tip)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FVector ProjectileSpawnOffset = FVector(100.0f, 0.0f, 20.0f);

	// --- Death Effects ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FZTDExplosionConfig ExplosionConfig;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	// Debug function to test Niagara system loading
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void TestNiagaraSystems();

	// Rotate actor to face direction, baking in MeshRotationOffset
	void FaceDirection(const FVector& Direction, float DeltaTime);

protected:
	virtual void BeginPlay() override;
	virtual void HandleDeath();
	virtual void FindTarget();

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	float FireCooldownTimer = 0.0f;
	bool bIsDead = false;
};
