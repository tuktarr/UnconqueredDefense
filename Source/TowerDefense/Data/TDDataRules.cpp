#include "Data/TDDataRules.h"
#include "BlueprintBridge/TDLegacyData.h"

using namespace TDLegacy;

void FTDDataRules::GetCurrentRoundData(UObject* LegacyContext, FStructProperty* Output, void* Address)
{
    RoundData(LegacyContext, Output, Address);
}

void FTDDataRules::InitializeTower(UObject* LegacyContext, FName RowName, FStructProperty* Output, void* Address)
{
    if (Output && Address)
    {
        FStructOnScope Default(Output->Struct);
        Output->Struct->CopyScriptStruct(Address,Default.GetStructMemory());
    }
    auto* Table=LoadObject<UDataTable>(nullptr,TEXT("/Game/Blueprint/Data/DT_TowerStatData.DT_TowerStatData"));
    auto* Stat=CastField<FStructProperty>(Property(LegacyContext,TEXT("Stat")));
    const uint8* Row=Table ? Table->FindRowUnchecked(RowName) : nullptr;
    if (Row && Stat && Output && Table->GetRowStruct()==Stat->Struct && Output->Struct==Stat->Struct)
    {
        Stat->Struct->CopyScriptStruct(Stat->ContainerPtrToValuePtr<void>(LegacyContext),Row);
        Output->Struct->CopyScriptStruct(Address,Row);
    }
}

void FTDDataRules::GetRandomTowerByRarity(UObject* LegacyContext, uint8 CurrentType, FStructProperty* Output, void* Address)
{
    if (Output) RandomTower(LegacyContext, CurrentType, Output->Struct, Address);
}
