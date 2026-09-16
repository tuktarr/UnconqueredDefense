#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDGridRules final
{
public:
    static void GenerateGrid(UObject* Context);
private:
    FTDGridRules() = delete;
};
