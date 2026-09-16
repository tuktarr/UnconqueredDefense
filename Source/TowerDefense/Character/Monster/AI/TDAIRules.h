#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDAIRules final
{
public:
    static void MonsterPossess(UObject* Context, APawn* PossessedPawn);
    static void ClearAITarget(UObject* Context, AAIController* OwnerController, APawn* ControlledPawn);
    static void FindNextCheckpoint(UObject* Context, AAIController* OwnerController, APawn* ControlledPawn, float DeltaSeconds);
private:
    FTDAIRules() = delete;
};
