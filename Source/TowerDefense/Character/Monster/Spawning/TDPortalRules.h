#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDPortalRules final
{
public:
    static void PortalBeginPlay(UObject* Context);
    static void PortalCreateMonster(UObject* Context);
    static void PortalSpawnMonster(UObject* Context);
    static void PortalResumeSpawning(UObject* Context);
    static void PortalResetBoss(UObject* Context);
private:
    FTDPortalRules() = delete;
};
