#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDRoundRules final
{
public:
    static void AddMonsterCount(UObject* Context);
    static void MinusMonsterCount(UObject* Context);
    static void GoNextRound(UObject* Context);
    static void UpdateRoundData(UObject* Context);
    static void GameModeTick(UObject* Context, float DeltaSeconds);
private:
    FTDRoundRules() = delete;
};
