#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDEconomyRules final
{
public:
    static void SpendGold(UObject* Context, double Amount, bool& Success);
    static void AddGold(UObject* Context, double Amount);
private:
    FTDEconomyRules() = delete;
};
