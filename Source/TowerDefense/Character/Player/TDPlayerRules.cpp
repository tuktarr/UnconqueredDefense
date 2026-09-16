#include "Character/Player/TDPlayerRules.h"
#include "Character/Tower/TDTowerRules.h"
#include "GameMode/Economy/TDEconomyRules.h"
#include "BlueprintBridge/TDLegacyData.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

using namespace TDLegacy;
namespace
{
bool IsClass(UObject* O,const TCHAR* Path)
{
    UClass* C=LoadClass<AActor>(nullptr,Path);
    return IsValid(O) && C && O->IsA(C);
}
const TCHAR* TowerPath=TEXT("/Game/Blueprint/Actor/Tower/BP_Tower_Base.BP_Tower_Base_C");
const TCHAR* SlotPath=TEXT("/Game/Blueprint/MapStructure/BP_TowerSlot.BP_TowerSlot_C");
struct FTowerStat
{
    FStructProperty* P;
    const void* Memory;
    explicit FTowerStat(UObject* Tower) : P(CastField<FStructProperty>(Property(Tower,TEXT("Stat")))), Memory(P ? P->ContainerPtrToValuePtr<void>(Tower) : nullptr) {}
    double Number(const TCHAR* N) const { return RowNumber(P ? P->Struct : nullptr,Memory,N); }
    UObject* Object(const TCHAR* N) const { return RowObject(P ? P->Struct : nullptr,Memory,N); }
};
AActor* SpawnRandomTower(UObject* Context,uint8 Rarity,const FVector& Location)
{
    UObject* GameMode=Object(Context,TEXT("GameMode"));
    auto* Table=Cast<UDataTable>(Object(GameMode,TEXT("TowerData")));
    if (!Table) return nullptr;
    FStructOnScope Row(Table->GetRowStruct());
    if (!RandomTower(GameMode,Rarity,Table->GetRowStruct(),Row.GetStructMemory())) return nullptr;
    auto* Class=Cast<UClass>(RowObject(Table->GetRowStruct(),Row.GetStructMemory(),TEXT("TowerClass")));
    if (!Class || !Context->GetWorld()) return nullptr;
    const FTransform Transform(FRotator::ZeroRotator,Location);
    AActor* Tower=Context->GetWorld()->SpawnActorDeferred<AActor>(Class,Transform,nullptr,nullptr,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!Tower) return nullptr;
    if (auto* Stat=CastField<FStructProperty>(Property(Tower,TEXT("Stat"))); Stat && Stat->Struct==Table->GetRowStruct())
        Stat->Struct->CopyScriptStruct(Stat->ContainerPtrToValuePtr<void>(Tower),Row.GetStructMemory());
    UGameplayStatics::FinishSpawningActor(Tower,Transform);
    return Tower;
}
void SlotAt(UObject* Context,const FVector& Location,bool Occupied)
{
    FHitResult Hit;
    UKismetSystemLibrary::LineTraceSingleForObjects(Context,Location+FVector(0,0,50),Location-FVector(0,0,100),
        {EObjectTypeQuery::ObjectTypeQuery10},false,{},EDrawDebugTrace::None,Hit,true);
    if (AActor* Slot=Hit.GetActor(); IsClass(Slot,SlotPath))
    {
        SetObject(Context,TEXT("CurrentTowerSlot"),Slot);
        SetBool(Slot,TEXT("bIsOccupied"),Occupied);
    }
}
void HideRange(UObject* Tower,bool Hidden)
{
    if (auto* Indicator=Cast<USceneComponent>(Object(Tower,TEXT("RangeIndicator")))) Indicator->SetHiddenInGame(Hidden,false);
}
}
void FTDPlayerRules::UpgradeTowerRank(UObject* Context,uint8 PreviousTower,uint8& UpgradeTower)
{
    UpgradeTower=PreviousTower<4 ? PreviousTower+1 : 0;
}
void FTDPlayerRules::ToggleBuildMode(UObject* Context)
{
    const int32 OldMode=Number(Context,TEXT("PlayerMode"));
    const int32 Mode=OldMode==0 ? 1 : (OldMode==1 ? 0 : OldMode);
    SetNumber(Context,TEXT("PlayerMode"),Mode);
    if (Mode!=1) { ClearGhostTower(Context); return; }
    if (!Context || !Context->GetWorld()) return;
    UClass* Class=LoadClass<AActor>(nullptr,TEXT("/Game/Blueprint/Actor/Tower/GhostTower/BP_GhostTower.BP_GhostTower_C"));
    if (Class) SetObject(Context,TEXT("CurrentGhostTower"),Context->GetWorld()->SpawnActor<AActor>(Class,FTransform::Identity));
}
void FTDPlayerRules::BeginMergeMode(UObject* Context) { SetNumber(Context,TEXT("PlayerMode"),2); ClearGhostTower(Context); }
void FTDPlayerRules::BeginSellMode(UObject* Context) { SetNumber(Context,TEXT("PlayerMode"),3); ClearGhostTower(Context); }
void FTDPlayerRules::EndTowerMode(UObject* Context) { SetNumber(Context,TEXT("PlayerMode"),0); }
void FTDPlayerRules::ControllerTick(UObject* Context)
{
    if (Number(Context,TEXT("PlayerMode"))==1) UpdateGhostTowerDisplay(Context);
}
void FTDPlayerRules::UpdateGhostTowerDisplay(UObject* Context)
{
    auto* PC=Cast<APlayerController>(Context);
    auto* Ghost=Cast<AActor>(Object(Context,TEXT("CurrentGhostTower")));
    if (!PC || !IsValid(Ghost)) return;
    FHitResult Cursor;
    if (!PC->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1,true,Cursor)) return;
    if (AActor* Hit=Cursor.GetActor()) Ghost->SetActorLocation(Hit->GetActorLocation());
    FHitResult Hit;
    UKismetSystemLibrary::SphereTraceSingle(Context,Ghost->GetActorLocation(),Ghost->GetActorLocation(),50,
        ETraceTypeQuery::TraceTypeQuery1,false,{},EDrawDebugTrace::None,Hit,true);
    const bool Buildable=Hit.bBlockingHit && Hit.GetActor() && Hit.GetActor()->ActorHasTag(TEXT("BuildArea"));
    if (auto* Material=Cast<UMaterialInstanceDynamic>(Object(Ghost,TEXT("GhostMaterialInst"))))
        Material->SetVectorParameterValue(TEXT("GhostColor"),Buildable ? FLinearColor::Green : FLinearColor::Red);
}
void FTDPlayerRules::ControllerClick(UObject* Context)
{
    auto* PC=Cast<APlayerController>(Context);
    if (!PC) return;
    FHitResult Hit;
    const bool HitSomething=PC->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1,true,Hit);
    ControllerInteract(Context,Hit.GetActor(),HitSomething);
}
void FTDPlayerRules::ControllerInteract(UObject* Context,AActor* Actor,bool HitSomething)
{
    const int32 Mode=Number(Context,TEXT("PlayerMode"));
    if (Mode==0)
    {
        UObject* Selected=Object(Context,TEXT("CurrentSelectedTower"));
        if (IsValid(Selected)) HideRange(Selected,true);
        if (IsClass(Actor,TowerPath) && Actor!=Selected)
        {
            SetObject(Context,TEXT("CurrentSelectedTower"),Actor); HideRange(Actor,false);
        }
        else SetObject(Context,TEXT("CurrentSelectedTower"),nullptr);
    }
    else if (Mode==1)
    {
        if (!HitSomething || !IsClass(Actor,SlotPath)) return;
        SetObject(Context,TEXT("CurrentTowerSlot"),Actor);
        auto* Ghost=Cast<AActor>(Object(Context,TEXT("CurrentGhostTower")));
        if (!IsValid(Ghost)) return;
        Ghost->SetActorLocation(Actor->GetActorLocation());
        if (Bool(Actor,TEXT("bIsOccupied"))) return;
        UObject* GameMode=Object(Context,TEXT("GameMode"));
        bool Paid=false; FTDEconomyRules::SpendGold(GameMode,Number(GameMode,TEXT("NormalTowerCost")),Paid);
        if (!Paid) return;
        SpawnRandomTower(Context,0,Ghost->GetActorLocation());
        SetBool(Actor,TEXT("bIsOccupied"),true);
        UpdateGhostTowerDisplay(Context);
    }
    else if (Mode==2)
    {
        SetBool(Context,TEXT("bIsMergeSuccess"),false);
        if (!IsClass(Actor,TowerPath)) return;
        const FTowerStat Selected(Actor);
        TArray<AActor*> Candidates;
        UGameplayStatics::GetAllActorsOfClass(Context,Cast<UClass>(Selected.Object(TEXT("TowerClass"))),Candidates);
        for (AActor* Other : Candidates)
        {
            if (!IsValid(Other) || Other==Actor) continue;
            const FTowerStat Material(Other);
            if (Material.Number(TEXT("Rarity"))!=Selected.Number(TEXT("Rarity")) || Material.Number(TEXT("TowerType"))!=Selected.Number(TEXT("TowerType"))) continue;
            SetBool(Context,TEXT("bIsMergeSuccess"),true);
            const FVector Location=Actor->GetActorLocation();
            SetVector(Context,TEXT("CurrentMergeLocation"),Location);
            SetNumber(Context,TEXT("CurrentRarity"),Selected.Number(TEXT("Rarity")));
            SetVector(Context,TEXT("CurrentMergeMaterialLocation"),Other->GetActorLocation());
            SlotAt(Context,Other->GetActorLocation(),false);
            Other->Destroy();
            uint8 Upgrade=0; UpgradeTowerRank(Context,Selected.Number(TEXT("Rarity")),Upgrade);
            AActor* NewTower=SpawnRandomTower(Context,Upgrade,Location);
            SlotAt(Context,Location,false);
            Actor->Destroy();
            if (NewTower) SlotAt(Context,NewTower->GetActorLocation(),true);
            break;
        }
    }
    else if (Mode==3 && HitSomething && IsClass(Actor,TowerPath))
    {
        const FTowerStat Stat(Actor);
        if (Stat.Number(TEXT("Rarity"))==4)
        {
            UKismetSystemLibrary::PrintString(Context,TEXT("Epic 판매 불가"));
            return;
        }
        struct { double Amount; } Args{Stat.Number(TEXT("SellPrice"))};
        Broadcast(Actor,TEXT("OnSellTower"),&Args);
        FTDTowerRules::DestroyAndFreeSlot(Actor);
    }
}

void FTDPlayerRules::ClearGhostTower(UObject* Context)
{
    if (AActor* Ghost = Cast<AActor>(Object(Context, TEXT("CurrentGhostTower"))); IsValid(Ghost)) Ghost->Destroy();
}
