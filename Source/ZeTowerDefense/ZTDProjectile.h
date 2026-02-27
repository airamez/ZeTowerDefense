#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZTDProjectile.generated.h"

UCLASS(Blueprintable)
class ZETOWERDEFENSE_API AZTDProjectile : public AActor
{
	GENERATED_BODY()

public:
	AZTDProjectile();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Adjust this if the rocket mesh faces the wrong direction (e.g. Yaw=90 or Yaw=-90)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FRotator MeshRotationOffset = FRotator(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Speed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float MaxLifeTime = 10.0f;

	void Initialize(AActor* InTarget, float InDamage, float InSpeed);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AActor* Target = nullptr;

	float LifeTimer = 0.0f;
};
