#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDMonsterRules final
{
public:
    static void MonsterActivate(UObject* Context);
    static void MonsterSpawnFromPool(UObject* Context);
    static void MonsterDeactivate(UObject* Context);
    static void MonsterDamage(UObject* Context,float Damage);
    static void MonsterReward(UObject* Context);
    static void MonsterRoundChanged(UObject* Context);
private:
    FTDMonsterRules() = delete;
};
