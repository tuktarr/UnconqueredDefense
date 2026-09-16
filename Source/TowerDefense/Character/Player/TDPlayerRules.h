#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDPlayerRules final
{
public:
    static void UpgradeTowerRank(UObject* Context,uint8 PreviousTower,uint8& UpgradeTower);
    static void ToggleBuildMode(UObject* Context);
    static void BeginMergeMode(UObject* Context);
    static void BeginSellMode(UObject* Context);
    static void EndTowerMode(UObject* Context);
    static void ControllerTick(UObject* Context);
    static void UpdateGhostTowerDisplay(UObject* Context);
    static void ControllerClick(UObject* Context);
    static void ControllerInteract(UObject* Context,AActor* Actor,bool HitSomething);
    static void ClearGhostTower(UObject* Context);
private:
    FTDPlayerRules() = delete;
};
