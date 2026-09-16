#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"

// Typed reflection at the legacy asset boundary, not a Blueprint graph interpreter.
namespace TDLegacy
{
    inline FProperty* Property(UObject* Object, FName Name)
    {
        return Object ? FindFProperty<FProperty>(Object->GetClass(), Name) : nullptr;
    }
    inline double Number(UObject* Object, FName Name)
    {
        const auto* P = CastField<FNumericProperty>(Property(Object, Name));
        if (!P) return 0.0;
        const void* Value = P->ContainerPtrToValuePtr<void>(Object);
        return P->IsInteger() ? static_cast<double>(P->GetSignedIntPropertyValue(Value)) : P->GetFloatingPointPropertyValue(Value);
    }
    inline void SetNumber(UObject* Object, FName Name, double Value)
    {
        if (auto* P = CastField<FNumericProperty>(Property(Object, Name)))
        {
            void* Address = P->ContainerPtrToValuePtr<void>(Object);
            if (P->IsInteger()) P->SetIntPropertyValue(Address, static_cast<int64>(Value));
            else P->SetFloatingPointPropertyValue(Address, Value);
        }
    }
    inline UObject* Object(UObject* Owner, FName Name)
    {
        auto* P = CastField<FObjectPropertyBase>(Property(Owner, Name));
        return P ? P->GetObjectPropertyValue_InContainer(Owner) : nullptr;
    }
    inline void SetObject(UObject* Owner, FName Name, UObject* Value)
    {
        if (auto* P = CastField<FObjectPropertyBase>(Property(Owner, Name))) P->SetObjectPropertyValue_InContainer(Owner, Value);
    }
    inline bool Bool(UObject* Owner, FName Name)
    {
        auto* P = CastField<FBoolProperty>(Property(Owner, Name));
        return P && P->GetPropertyValue_InContainer(Owner);
    }
    inline void SetBool(UObject* Owner, FName Name, bool Value)
    {
        if (auto* P = CastField<FBoolProperty>(Property(Owner, Name))) P->SetPropertyValue_InContainer(Owner, Value);
    }
    inline FName Name(UObject* Owner, FName Field)
    {
        auto* P = CastField<FNameProperty>(Property(Owner, Field));
        return P ? P->GetPropertyValue_InContainer(Owner) : NAME_None;
    }
    inline void Call(UObject* Owner, FName FunctionName)
    {
        if (Owner) if (UFunction* F = Owner->FindFunction(FunctionName))
        {
            FStructOnScope Params(F);
            Owner->ProcessEvent(F, Params.GetStructMemory());
        }
    }
    inline void Broadcast(UObject* Owner, FName Name, void* Params = nullptr)
    {
        if (auto* P = CastField<FMulticastDelegateProperty>(Property(Owner, Name)))
            P->GetMulticastDelegate(P->ContainerPtrToValuePtr<void>(Owner))->ProcessDelegate<UObject>(Params);
    }
    inline TArray<UObject*> Objects(UObject* Owner, FName Name)
    {
        TArray<UObject*> Values;
        if (auto* P = CastField<FArrayProperty>(Property(Owner, Name)))
        {
            auto* Inner = CastField<FObjectPropertyBase>(P->Inner);
            if (!Inner) return Values;
            FScriptArrayHelper Array(P, P->ContainerPtrToValuePtr<void>(Owner));
            for (int32 I=0; I<Array.Num(); ++I) Values.Add(Inner->GetObjectPropertyValue(Array.GetRawPtr(I)));
        }
        return Values;
    }
    inline void RemoveFirst(UObject* Owner, FName Name)
    {
        if (auto* P = CastField<FArrayProperty>(Property(Owner, Name)))
        {
            FScriptArrayHelper Array(P, P->ContainerPtrToValuePtr<void>(Owner));
            if (Array.Num()) Array.RemoveValues(0);
        }
    }
    inline void SetVector(UObject* Owner, FName Name, const FVector& Value)
    {
        auto* P = CastField<FStructProperty>(Property(Owner, Name));
        if (P && P->Struct == TBaseStructure<FVector>::Get()) *P->ContainerPtrToValuePtr<FVector>(Owner) = Value;
    }
inline bool IsMonster(AActor* Actor)
{
    UClass* Class = LoadClass<AActor>(nullptr, TEXT("/Game/Blueprint/Actor/Monster/BP_Monster.BP_Monster_C"));
    return IsValid(Actor) && Class && Actor->IsA(Class);
}
}
