#include "BlueprintBridge/TDGameplayLibrary.h"
#include "BlueprintBridge/TDLegacyData.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"

// Explicit opt-in integration fixture. Never active in normal play or shipping.
#if !UE_BUILD_SHIPPING
namespace
{
FDelegateHandle TickHandle;
using namespace TDLegacy;
struct FSmoke
{
    int32 Stage=0, Checks=0, Failures=0;
    float NextTime=3;
    TWeakObjectPtr<AActor> FirstTower, SecondTower, Upgraded, FirstSlot;
    void Expect(bool OK,const TCHAR* Label)
    {
        ++Checks;
        if (OK) { UE_LOG(LogTemp,Display,TEXT("TD_SMOKE PASS %s"),Label); }
        else { ++Failures; UE_LOG(LogTemp,Error,TEXT("TD_SMOKE FAIL %s"),Label); }
    }
    TArray<AActor*> Actors(UWorld* World,const TCHAR* ClassPath)
    {
        TArray<AActor*> Result;
        if (UClass* C=LoadClass<AActor>(nullptr,ClassPath)) UGameplayStatics::GetAllActorsOfClass(World,C,Result);
        return Result;
    }
    TArray<AActor*> Towers(UWorld* W) { return Actors(W,TEXT("/Game/Blueprint/Actor/Tower/BP_Tower_Base.BP_Tower_Base_C")); }
    TArray<AActor*> Monsters(UWorld* W) { return Actors(W,TEXT("/Game/Blueprint/Actor/Monster/BP_Monster.BP_Monster_C")); }
    void Tick(UWorld* W)
    {
        if (!W || !W->IsGameWorld() || !W->HasBegunPlay() || W->GetTimeSeconds()<NextTime) return;
        UObject* GM=W->GetAuthGameMode();
        auto* PC=W->GetFirstPlayerController();
        if (!GM || !PC) return;
        if (Stage==0)
        {
            auto Portals=Actors(W,TEXT("/Game/Blueprint/Actor/PoolManager/BP_MonsterPortal.BP_MonsterPortal_C"));
            Expect(Portals.Num()>0,TEXT("MainLevel contains portal"));
            if (!Portals.IsEmpty())
            {
                Expect(Objects(Portals[0],TEXT("MonsterPool")).Num()==Number(Portals[0],TEXT("PoolSize"))-1,TEXT("Pool prewarm count"));
                Expect(Objects(Portals[0],TEXT("WaypointList")).Num()==4,TEXT("Four ordered waypoints"));
            }
            Expect(Number(GM,TEXT("MonsterCount"))>0,TEXT("Spawn timer activates monsters"));
            auto Slots=Actors(W,TEXT("/Game/Blueprint/MapStructure/BP_TowerSlot.BP_TowerSlot_C"));
            Expect(Slots.Num()>=2,TEXT("Grid creates build slots"));
            if (Slots.Num()<2) { Finish(); return; }
            FirstSlot=Slots[0];
            SetNumber(GM,TEXT("Gold"),500);
            SetNumber(PC,TEXT("PlayerMode"),0);
            UTDGameplayLibrary::ToggleBuildMode(PC);
            FMath::RandInit(23); UTDGameplayLibrary::ControllerInteract(PC,Slots[0],true);
            auto Before=Towers(W); if (!Before.IsEmpty()) FirstTower=Before.Last();
            FMath::RandInit(23); UTDGameplayLibrary::ControllerInteract(PC,Slots[1],true);
            for (AActor* T : Towers(W)) if (!Before.Contains(T)) SecondTower=T;
            Expect(FirstTower.IsValid() && SecondTower.IsValid(),TEXT("Build creates two initialized towers"));
            Expect(Number(GM,TEXT("Gold"))==500-2*Number(GM,TEXT("NormalTowerCost")),TEXT("Build gold accounting"));
            UTDGameplayLibrary::BeginMergeMode(PC);
            NextTime=W->GetTimeSeconds()+0.5f;
        }
        else if (Stage==1)
        {
            if (FirstTower.IsValid()) UTDGameplayLibrary::ControllerInteract(PC,FirstTower.Get(),true);
            Expect(Bool(PC,TEXT("bIsMergeSuccess")),TEXT("Matching towers merge"));
            Expect(!FirstTower.IsValid() && !SecondTower.IsValid(),TEXT("Merge consumes both old towers"));
            auto Current=Towers(W);
            for (AActor* T : Current)
            {
                auto* P=CastField<FStructProperty>(Property(T,TEXT("Stat")));
                if (P && RowNumber(P->Struct,P->ContainerPtrToValuePtr<void>(T),TEXT("Rarity"))==1) Upgraded=T;
            }
            Expect(Upgraded.IsValid(),TEXT("Merge produces next rarity"));
            NextTime=W->GetTimeSeconds()+0.5f;
        }
        else if (Stage==2)
        {
            if (Upgraded.IsValid())
            {
                auto* P=CastField<FStructProperty>(Property(Upgraded.Get(),TEXT("Stat")));
                const double Price=RowNumber(P->Struct,P->ContainerPtrToValuePtr<void>(Upgraded.Get()),TEXT("SellPrice"));
                const double Gold=Number(GM,TEXT("Gold"));
                UTDGameplayLibrary::BeginSellMode(PC);
                UTDGameplayLibrary::ControllerInteract(PC,Upgraded.Get(),true);
                Expect(Number(GM,TEXT("Gold"))==Gold+Price,TEXT("Sale delegate awards configured price"));
                Expect(!Upgraded.IsValid(),TEXT("Sale removes tower"));
            }
            Expect(FirstSlot.IsValid() && !Bool(FirstSlot.Get(),TEXT("bIsOccupied")),TEXT("Sale frees original slot"));
            bool Tested=false;
            for (AActor* M : Monsters(W)) if (Bool(M,TEXT("IsActive")))
            {
                UObject* Health=Object(M,TEXT("HP_Controller"));
                Expect(Number(Health,TEXT("HP"))>0,TEXT("Active monster receives round health"));
                const double Gold=Number(GM,TEXT("Gold")), Count=Number(GM,TEXT("MonsterCount")), Reward=Number(M,TEXT("RewardGold"));
                UGameplayStatics::ApplyDamage(M,100000,nullptr,nullptr,nullptr);
                Expect(Number(GM,TEXT("Gold"))==Gold+Reward && Number(GM,TEXT("MonsterCount"))==Count-1,TEXT("Damage death reward and count"));
                UGameplayStatics::ApplyDamage(M,100000,nullptr,nullptr,nullptr);
                Expect(Number(GM,TEXT("Gold"))==Gold+Reward && Number(GM,TEXT("MonsterCount"))==Count-1,TEXT("Repeated damage cannot duplicate reward"));
                Tested=true; break;
            }
            Expect(Tested,TEXT("At least one live damage target"));
            NextTime=W->GetTimeSeconds()+1.5f;
        }
        else if (Stage==3)
        {
            // Test round signals with all current monsters inactive. No saved data is changed.
            for (AActor* M : Monsters(W)) UTDGameplayLibrary::MonsterDeactivate(M);
            SetNumber(GM,TEXT("MonsterCount"),0);
            SetNumber(GM,TEXT("CurrentRound"),4);
            UTDGameplayLibrary::GoNextRound(GM);
            Expect(Number(GM,TEXT("CurrentRound"))==5 && Number(GM,TEXT("RemainingTime"))==30,TEXT("Round signal and timer reset"));
            for (AActor* M : Monsters(W)) Expect(Number(Object(M,TEXT("MonsterData")),TEXT("CurrentRound"))==5,TEXT("Pooled monster follows round signal"));
            NextTime=W->GetTimeSeconds()+2;
        }
        else
        {
            Expect(Number(GM,TEXT("MonsterCount"))>0,TEXT("Spawning continues after round change"));
            Finish(); return;
        }
        ++Stage;
    }
    void Finish()
    {
        UE_LOG(LogTemp,Display,TEXT("TD_SMOKE checks=%d failures=%d"),Checks,Failures);
        NextTime=MAX_flt;
        FPlatformMisc::RequestExitWithStatus(false,Failures ? 1 : 0);
    }
};
TSharedPtr<FSmoke> Smoke;
}
#endif
void StartTDMigrationSmoke()
{
#if !UE_BUILD_SHIPPING
    if (!FParse::Param(FCommandLine::Get(),TEXT("TDGameplaySmoke"))) return;
    Smoke=MakeShared<FSmoke>();
    TickHandle=FWorldDelegates::OnWorldPostActorTick.AddLambda([](UWorld* W,ELevelTick,float) { if (Smoke) Smoke->Tick(W); });
#endif
}
void StopTDMigrationSmoke()
{
#if !UE_BUILD_SHIPPING
    FWorldDelegates::OnWorldPostActorTick.Remove(TickHandle); Smoke.Reset();
#endif
}
