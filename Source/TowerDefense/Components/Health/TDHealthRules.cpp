#include "Components/Health/TDHealthRules.h"
#include "BlueprintBridge/TDLegacyData.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

using namespace TDLegacy;
void FTDHealthRules::InitializeHealth(UObject* Context)
{
    SetNumber(Context, TEXT("HP"), Number(Context, TEXT("MaxHP")));
}
void FTDHealthRules::UpdateMaxHealth(UObject* Context, double MaxHP)
{
    SetNumber(Context, TEXT("MaxHP"), MaxHP);
    SetNumber(Context, TEXT("HP"), MaxHP);
}
void FTDHealthRules::ReceiveDamage(UObject* Context, double Damage)
{
    if (Number(Context, TEXT("HP")) <= 0) return;
    const double HP = FMath::Max(0.0, Number(Context, TEXT("HP")) - Damage);
    SetNumber(Context, TEXT("HP"), HP);
    auto* Component = Cast<UActorComponent>(Context);
    UObject* Widget = Component ? Object(Component->GetOwner(), TEXT("MonsterHPBar")) : nullptr;
    const double MaxHP = Number(Context, TEXT("MaxHP"));
    if (Widget) if (UFunction* F = Widget->FindFunction(TEXT("UpdateHp")))
    {
        FStructOnScope Params(F);
        if (auto* P = FindFProperty<FDoubleProperty>(F, TEXT("CurrentHp"))) P->SetPropertyValue_InContainer(Params.GetStructMemory(), HP);
        if (auto* P = FindFProperty<FDoubleProperty>(F, TEXT("MaxHp"))) P->SetPropertyValue_InContainer(Params.GetStructMemory(), MaxHP);
        Widget->ProcessEvent(F, Params.GetStructMemory());
    }
    if (HP <= 0) Broadcast(Context, TEXT("OnDeadEvent"));
    else { struct { double HP; double MaxHP; } Values{HP, MaxHP}; Broadcast(Context, TEXT("OnHpChanged"), &Values); }
}
