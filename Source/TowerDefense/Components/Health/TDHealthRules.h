#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDHealthRules final
{
public:
    static void InitializeHealth(UObject* Context);
    static void UpdateMaxHealth(UObject* Context, double MaxHP);
    static void ReceiveDamage(UObject* Context, double Damage);
private:
    FTDHealthRules() = delete;
};
