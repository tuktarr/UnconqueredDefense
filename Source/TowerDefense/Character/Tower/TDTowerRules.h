#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDTowerRules final
{
public:
    static void TowerBeginOverlap(UObject* Context, AActor* OtherActor);
    static void TowerEndOverlap(UObject* Context, AActor* OtherActor);
    static void TowerAttackNotify(UObject* Context);
    static void TowerAttackEndNotify(UObject* Context);
    static void DestroyAndFreeSlot(UObject* Context);
    static void TowerBossSpawned(UObject* Context);
    static void TowerSold(UObject* Context,double Amount);
private:
    FTDTowerRules() = delete;
};
