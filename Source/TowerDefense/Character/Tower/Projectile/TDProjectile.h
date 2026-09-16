#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDProjectile : public AActor
{
    GENERATED_BODY()

public:
    ATDProjectile();
    UFUNCTION(BlueprintCallable, Category="Projectile") void ActivateProjectile(const FTransform& Transform, double Damage, AActor* NewOwner);
    UFUNCTION(BlueprintCallable, Category="Projectile") void DeactivateProjectile();

protected:
    UFUNCTION() void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USphereComponent> Collision;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile") double ProjectileDamage = 0.0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile") bool bIsActive = false;
};
