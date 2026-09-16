#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDProjectileRules final
{
public:
    static void ProjectileOverlap(UObject* Context, AActor* OtherActor);
    static void CannonOverlap(UObject* Context, AActor* OtherActor);
    static void ActivateProjectile(UObject* Context);
    static void DeactivateProjectile(UObject* Context);
private:
    FTDProjectileRules() = delete;
};
