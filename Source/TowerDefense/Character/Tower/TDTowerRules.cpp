#include "Character/Tower/TDTowerRules.h"
#include "GameMode/Economy/TDEconomyRules.h"
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
namespace
{
void SetTowerTickEnabled(UObject* Context, const bool bEnabled)
{
    if (AActor* Tower = Cast<AActor>(Context))
    {
        Tower->SetActorTickEnabled(bEnabled);
    }
}
}

void FTDTowerRules::TowerBeginOverlap(UObject* Context, AActor* OtherActor)
{
    if (!IsMonster(OtherActor)) return;
    auto* P = CastField<FArrayProperty>(Property(Context, TEXT("TargetArray")));
    auto* Inner = P ? CastField<FObjectPropertyBase>(P->Inner) : nullptr;
    if (!Inner) return;
    FScriptArrayHelper Array(P, P->ContainerPtrToValuePtr<void>(Context));
    bool Found = false;
    for (int32 I=0; I<Array.Num(); ++I) Found |= Inner->GetObjectPropertyValue(Array.GetRawPtr(I)) == OtherActor;
    if (!Found) Inner->SetObjectPropertyValue(Array.GetRawPtr(Array.AddValue()), OtherActor);
    if (Array.Num()==1)
    {
        SetBool(Context, TEXT("bIsActioning"), true);
        SetTowerTickEnabled(Context, true);
    }
}
void FTDTowerRules::TowerEndOverlap(UObject* Context, AActor* OtherActor)
{
    if (!IsMonster(OtherActor)) return;
    auto* P = CastField<FArrayProperty>(Property(Context, TEXT("TargetArray")));
    auto* Inner = P ? CastField<FObjectPropertyBase>(P->Inner) : nullptr;
    if (!Inner) return;
    FScriptArrayHelper Array(P, P->ContainerPtrToValuePtr<void>(Context));
    for (int32 I=Array.Num()-1; I>=0; --I)
        if (Inner->GetObjectPropertyValue(Array.GetRawPtr(I))==OtherActor) Array.RemoveValues(I);
    if (Array.Num()==0) SetTowerTickEnabled(Context, false);
    // The original waits for the animation-end notification to update this flag.
}
void FTDTowerRules::TowerAttackNotify(UObject* Context)
{
    if (Bool(Context, TEXT("bHasAttacked"))) return;
    Call(Context, TEXT("TowerAction"));
    SetBool(Context, TEXT("bHasAttacked"), true);
}
void FTDTowerRules::TowerAttackEndNotify(UObject* Context)
{
    SetBool(Context, TEXT("bHasAttacked"), false);
    const bool bHasTargets = !Objects(Context, TEXT("TargetArray")).IsEmpty();
    SetBool(Context, TEXT("bIsActioning"), bHasTargets);
    if (!bHasTargets) SetTowerTickEnabled(Context, false);
}
void FTDTowerRules::DestroyAndFreeSlot(UObject* Context)
{
    auto* Tower = Cast<AActor>(Context);
    if (!Tower) return;
    FHitResult Hit;
    const FVector Location=Tower->GetActorLocation();
    UKismetSystemLibrary::LineTraceSingleForObjects(Context, Location+FVector(0,0,50), Location-FVector(0,0,100),
        {EObjectTypeQuery::ObjectTypeQuery10}, false, {}, EDrawDebugTrace::None, Hit, true);
    UClass* SlotClass=LoadClass<AActor>(nullptr, TEXT("/Game/Blueprint/MapStructure/BP_TowerSlot.BP_TowerSlot_C"));
    if (AActor* Slot=Hit.GetActor(); Slot && SlotClass && Slot->IsA(SlotClass)) SetBool(Slot,TEXT("bIsOccupied"),false);
    Tower->Destroy();
}
void FTDTowerRules::TowerBossSpawned(UObject* Context)
{
    auto* P=CastField<FStructProperty>(Property(Context,TEXT("Stat")));
    if (!P) return;
    const double Rarity=RowNumber(P->Struct,P->ContainerPtrToValuePtr<void>(Context),TEXT("Rarity"));
    if (Rarity==0 || Rarity==1) DestroyAndFreeSlot(Context);
}
void FTDTowerRules::TowerSold(UObject* Context,double Amount)
{
    FTDEconomyRules::AddGold(UGameplayStatics::GetGameMode(Context),Amount);
}
