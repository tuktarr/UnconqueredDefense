#pragma once

#include "CoreMinimal.h"

class FStructProperty;

// Data operations independent of Blueprint VM argument decoding.
class TOWERDEFENSE_API FTDDataRules final
{
public:
    static void GetCurrentRoundData(UObject* Context, FStructProperty* Output, void* Address);
    static void InitializeTower(UObject* Context, FName RowName, FStructProperty* Output, void* Address);
    static void GetRandomTowerByRarity(UObject* Context, uint8 CurrentType, FStructProperty* Output, void* Address);

private:
    FTDDataRules() = delete;
};
