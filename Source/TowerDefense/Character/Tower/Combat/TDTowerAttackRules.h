#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDTowerAttackRules final
{
public:
    static void SwordAction(UObject* Context);
    static void GunAction(UObject* Context);
    static void RobotAction(UObject* Context);
    static void MagicAction(UObject* Context);
    static void SupportAction(UObject* Context);
private:
    FTDTowerAttackRules() = delete;
};
