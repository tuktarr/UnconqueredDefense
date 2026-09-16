#pragma once
#include "BlueprintBridge/TDLegacyAccess.h"
#include "Engine/DataTable.h"

namespace TDLegacy
{
    inline FProperty* Field(const UStruct* Type, const TCHAR* Prefix)
    {
        if (Type) for (TFieldIterator<FProperty> P(Type); P; ++P)
            if (P->GetName()==Prefix || P->GetName().StartsWith(FString(Prefix)+TEXT("_"))) return *P;
        return nullptr;
    }
    inline double RowNumber(const UStruct* Type, const void* Row, const TCHAR* Prefix)
    {
        auto* P=CastField<FNumericProperty>(Field(Type,Prefix));
        if (!P || !Row) return 0;
        const void* V=P->ContainerPtrToValuePtr<void>(Row);
        return P->IsInteger() ? P->GetSignedIntPropertyValue(V) : P->GetFloatingPointPropertyValue(V);
    }
    inline UObject* RowObject(const UStruct* Type, const void* Row, const TCHAR* Prefix)
    {
        auto* P=CastField<FObjectPropertyBase>(Field(Type,Prefix));
        return P && Row ? P->GetObjectPropertyValue_InContainer(Row) : nullptr;
    }
    inline bool RowBool(const UStruct* Type, const void* Row, const TCHAR* Prefix)
    {
        auto* P=CastField<FBoolProperty>(Field(Type,Prefix));
        return P && Row && P->GetPropertyValue_InContainer(Row);
    }
    inline void RoundData(UObject* Component, FStructProperty* Output, void* Destination)
    {
        if (!Component || !Output || !Destination) return;
        FStructOnScope Default(Output->Struct);
        Output->Struct->CopyScriptStruct(Destination,Default.GetStructMemory());
        auto* Table=Cast<UDataTable>(Object(Component,TEXT("MonsterData")));
        if (!Table || Table->GetRowStruct()!=Output->Struct) return;
        const uint8* Row=Table->FindRowUnchecked(FName(*FString::Printf(TEXT("Monster_%d"),static_cast<int32>(Number(Component,TEXT("CurrentRound"))))));
        if (Row) Output->Struct->CopyScriptStruct(Destination,Row);
    }
    // Keeps the original user-defined structure (including its default values).
    struct FRoundData
    {
        FStructProperty* Output;
        FStructOnScope Data;
        explicit FRoundData(UObject* Component)
            : Output(Component ? FindFProperty<FStructProperty>(Component->FindFunction(TEXT("GetCurrentRoundData")),TEXT("CurrentRoundData")) : nullptr),
              Data(Output ? Output->Struct : nullptr)
        {
            RoundData(Component,Output,Data.GetStructMemory());
        }
        UScriptStruct* Type() const { return Output ? Output->Struct : nullptr; }
        const void* Memory() const { return Data.GetStructMemory(); }
        double Number(const TCHAR* N) const { return RowNumber(Type(),Memory(),N); }
        bool Bool(const TCHAR* N) const { return RowBool(Type(),Memory(),N); }
        UObject* Object(const TCHAR* N) const { return RowObject(Type(),Memory(),N); }
    };
    inline bool RandomTower(UObject* GameMode, uint8 Rarity, const UScriptStruct* Type, void* Destination)
    {
        if (!Type || !Destination) return false;
        FStructOnScope Default(Type);
        Type->CopyScriptStruct(Destination,Default.GetStructMemory());
        auto* P=CastField<FArrayProperty>(Property(GameMode,TEXT("LocalTowerArray")));
        auto* Inner=P ? CastField<FStructProperty>(P->Inner) : nullptr;
        auto* Table=Cast<UDataTable>(Object(GameMode,TEXT("TowerData")));
        if (!Inner || !Type || !Destination || Type!=Inner->Struct) return false;
        FScriptArrayHelper Array(P,P->ContainerPtrToValuePtr<void>(GameMode));
        Array.EmptyValues();
        if (!Table || Table->GetRowStruct()!=Type) return false;
        for (const FName Name : Table->GetRowNames())
        {
            const uint8* Row=Table->FindRowUnchecked(Name);
            if (!Row || RowNumber(Type,Row,TEXT("Rarity"))!=Rarity) continue;
            bool Exists=false;
            for (int32 I=0; I<Array.Num(); ++I) Exists |= Inner->Identical(Array.GetRawPtr(I),Row,PPF_None);
            if (!Exists) Type->CopyScriptStruct(Array.GetRawPtr(Array.AddValue()),Row);
        }
        if (!Array.Num()) return false;
        Type->CopyScriptStruct(Destination,Array.GetRawPtr(FMath::RandRange(0,Array.Num()-1)));
        return true;
    }
}
