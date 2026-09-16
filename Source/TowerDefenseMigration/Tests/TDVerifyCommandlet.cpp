#include "Tests/TDVerifyCommandlet.h"
#include "BlueprintBridge/TDGameplayLibrary.h"
#include "Engine/World.h"
#include "Engine/Blueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "KismetCompiler.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "UObject/Script.h"
#include "BlueprintBridge/TDLegacyData.h"
#include "EngineUtils.h"

namespace
{
double Read(UObject* O, FName N)
{
    auto* P=FindFProperty<FNumericProperty>(O->GetClass(),N);
    check(P);
    const void* V=P->ContainerPtrToValuePtr<void>(O);
    return P->IsInteger() ? P->GetSignedIntPropertyValue(V) : P->GetFloatingPointPropertyValue(V);
}
void Write(UObject* O, FName N, double V)
{
    auto* P=FindFProperty<FNumericProperty>(O->GetClass(),N); check(P);
    void* D=P->ContainerPtrToValuePtr<void>(O);
    if (P->IsInteger()) P->SetIntPropertyValue(D,static_cast<int64>(V)); else P->SetFloatingPointPropertyValue(D,V);
}
bool Invoke(UObject* O, FName Name, FName Input=NAME_None, double Value=0)
{
    UFunction* F=O->FindFunction(Name); check(F);
    FStructOnScope Args(F);
    if (!Input.IsNone())
    {
        auto* P=FindFProperty<FNumericProperty>(F,Input); check(P);
        void* D=P->ContainerPtrToValuePtr<void>(Args.GetStructMemory());
        if(P->IsInteger()) P->SetIntPropertyValue(D,static_cast<int64>(Value)); else P->SetFloatingPointPropertyValue(D,Value);
    }
    O->ProcessEvent(F,Args.GetStructMemory());
    auto* Success=FindFProperty<FBoolProperty>(F,TEXT("Success"));
    return Success && Success->GetPropertyValue_InContainer(Args.GetStructMemory());
}
void Bind(UObject* O, FName Name, UObject* Listener, FName Function)
{
    auto* P=FindFProperty<FMulticastDelegateProperty>(O->GetClass(),Name); check(P);
    FScriptDelegate D; D.BindUFunction(Listener,Function);
    P->AddDelegate(D,O);
}
}
UTDVerifyCommandlet::UTDVerifyCommandlet()
{
    IsEditor=true; IsClient=false; IsServer=false; LogToConsole=true;
}
int32 UTDVerifyCommandlet::Main(const FString& Params)
{
    FEditorScriptExecutionGuard ScriptGuard;
    const auto Settings=UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
    UWorld* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Settings);
    int32 Checks=0, Failures=0;
    auto Expect=[&](bool OK,const FString& Label)
    {
        ++Checks;
        if (!OK) { ++Failures; UE_LOG(LogTemp,Error,TEXT("FAIL %s"),*Label); }
        else UE_LOG(LogTemp,Display,TEXT("PASS %s"),*Label);
    };
    auto Spawn=[&](const TCHAR* Path)->AActor*
    {
        UClass* C=LoadClass<AActor>(nullptr,Path); check(C);
        FActorSpawnParameters P; P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        return World->SpawnActor<AActor>(C,FTransform::Identity,P);
    };
    UObject* BP=Spawn(TEXT("/Game/Blueprint/BP_TowerDefenseGameMode.BP_TowerDefenseGameMode_C"));
    UObject* Native=Spawn(TEXT("/Game/Blueprint/BP_TowerDefenseGameMode.BP_TowerDefenseGameMode_C"));
    for (double Amount : {0.0,10.0,50.0,51.0,-5.0})
    {
        Write(BP,TEXT("Gold"),50); Write(Native,TEXT("Gold"),50);
        bool Actual=false; UTDGameplayLibrary::SpendGold(Native,Amount,Actual);
        const bool Expected=Invoke(BP,TEXT("SpendGold"),TEXT("Amount"),Amount);
        Expect(Expected==Actual && Read(BP,TEXT("Gold"))==Read(Native,TEXT("Gold")),FString::Printf(TEXT("SpendGold %.1f parity"),Amount));
    }
    for (double Amount : {-3.0,0.0,12.5})
    {
        Write(BP,TEXT("Gold"),50); Write(Native,TEXT("Gold"),50);
        Invoke(BP,TEXT("AddGold"),TEXT("Amount"),Amount); UTDGameplayLibrary::AddGold(Native,Amount);
        Expect(Read(BP,TEXT("Gold"))==Read(Native,TEXT("Gold")),FString::Printf(TEXT("AddGold %.1f parity"),Amount));
    }
    Write(BP,TEXT("MonsterCount"),0); Write(Native,TEXT("MonsterCount"),0);
    Invoke(BP,TEXT("MinusMonsterCount")); UTDGameplayLibrary::MinusMonsterCount(Native);
    Expect(Read(BP,TEXT("MonsterCount"))==-1 && Read(Native,TEXT("MonsterCount"))==-1,TEXT("Count preserves original unclamped decrement"));
    Invoke(BP,TEXT("AddMonsterCount")); UTDGameplayLibrary::AddMonsterCount(Native);
    Expect(Read(BP,TEXT("MonsterCount"))==Read(Native,TEXT("MonsterCount")),TEXT("Count increment parity"));
    Write(BP,TEXT("CurrentRound"),1); Write(Native,TEXT("CurrentRound"),1);
    Invoke(BP,TEXT("GoNextRound")); UTDGameplayLibrary::GoNextRound(Native);
    Expect(Read(BP,TEXT("CurrentRound"))==2 && Read(Native,TEXT("CurrentRound"))==2,TEXT("Round increment parity"));
    Expect(Read(BP,TEXT("RemainingTime"))==30 && Read(Native,TEXT("RemainingTime"))==30,TEXT("Round resets timer to 30"));
    for (int32 Round=1; Round<=10; ++Round)
    {
        Write(BP,TEXT("CurrentRound"),Round); Write(Native,TEXT("CurrentRound"),Round);
        Invoke(BP,TEXT("UpdateRoundData")); UTDGameplayLibrary::UpdateRoundData(Native);
        auto* P=FindFProperty<FStructProperty>(BP->GetClass(),TEXT("CurrentRoundData")); check(P);
        Expect(P->Identical_InContainer(BP,Native),FString::Printf(TEXT("Round %d complete table row parity"),Round));
    }
    UClass* HealthClass=LoadClass<UObject>(nullptr,TEXT("/Game/Blueprint/Actor/Monster/BP_HP_Controller.BP_HP_Controller_C")); check(HealthClass);
    UObject* HP=NewObject<UObject>(GetTransientPackage(),HealthClass);
    UObject* OriginalHP=NewObject<UObject>(GetTransientPackage(),HealthClass);
    Invoke(OriginalHP,TEXT("UpdateMaxHP"),TEXT("MaxHP"),75); UTDGameplayLibrary::UpdateMaxHealth(HP,75);
    Expect(Read(HP,TEXT("HP"))==Read(OriginalHP,TEXT("HP")) && Read(HP,TEXT("MaxHP"))==Read(OriginalHP,TEXT("MaxHP")),TEXT("Max health reset parity"));
    Write(HP,TEXT("HP"),5); Write(OriginalHP,TEXT("HP"),5);
    Invoke(OriginalHP,TEXT("InitHP")); UTDGameplayLibrary::InitializeHealth(HP);
    Expect(Read(HP,TEXT("HP"))==Read(OriginalHP,TEXT("HP")),TEXT("Health initialization parity"));
    Bind(HP,TEXT("OnDeadEvent"),this,TEXT("OnDeath")); Bind(HP,TEXT("OnHpChanged"),this,TEXT("OnHealth"));
    UTDGameplayLibrary::ReceiveDamage(HP,25);
    Expect(Read(HP,TEXT("HP"))==50 && HealthChanges==1 && Deaths==0 && LastHP==50 && LastMaxHP==75,TEXT("Nonlethal damage and delegate parameters"));
    UTDGameplayLibrary::ReceiveDamage(HP,100);
    Expect(Read(HP,TEXT("HP"))==0 && HealthChanges==1 && Deaths==1,TEXT("Lethal damage clamps to zero and emits one death"));
    UTDGameplayLibrary::ReceiveDamage(HP,100);
    Expect(Deaths==1 && Read(HP,TEXT("HP"))==0,TEXT("Dead target cannot award a second death"));
    UObject* Tower=Spawn(TEXT("/Game/Blueprint/Actor/Tower/BP_Tower_Base.BP_Tower_Base_C"));
    auto* Attacked=FindFProperty<FBoolProperty>(Tower->GetClass(),TEXT("bHasAttacked")); check(Attacked);
    UTDGameplayLibrary::TowerAttackNotify(Tower);
    Expect(Attacked->GetPropertyValue_InContainer(Tower),TEXT("Attack notify marks attack"));
    UTDGameplayLibrary::TowerAttackNotify(Tower);
    UTDGameplayLibrary::TowerAttackEndNotify(Tower);
    auto* Action=FindFProperty<FBoolProperty>(Tower->GetClass(),TEXT("bIsActioning")); check(Action);
    Expect(!Attacked->GetPropertyValue_InContainer(Tower) && !Action->GetPropertyValue_InContainer(Tower),TEXT("Attack end resets empty target state"));
    // With a fixed seed, compare all returned fields, not just the rarity.
    UFunction* Pick=BP->FindFunction(TEXT("GetRandomTowerByRarity")); check(Pick);
    auto* Input=FindFProperty<FByteProperty>(Pick,TEXT("CurrentType")); check(Input);
    auto* Output=FindFProperty<FStructProperty>(Pick,TEXT("SelectedTower")); check(Output);
    for (int32 Rarity=0; Rarity<5; ++Rarity) for (int32 Seed=0; Seed<10; ++Seed)
    {
        FStructOnScope Args(Pick), Selected(Output->Struct);
        Input->SetPropertyValue_InContainer(Args.GetStructMemory(),Rarity);
        FMath::RandInit(Seed); BP->ProcessEvent(Pick,Args.GetStructMemory());
        FMath::RandInit(Seed); const bool Found=TDLegacy::RandomTower(Native,Rarity,Output->Struct,Selected.GetStructMemory());
        Expect(Found && Output->Identical(Output->ContainerPtrToValuePtr<void>(Args.GetStructMemory()),Selected.GetStructMemory(),PPF_None),
            FString::Printf(TEXT("Random tower complete row parity rarity=%d seed=%d"),Rarity,Seed));
    }
    UObject* PC=Spawn(TEXT("/Game/Blueprint/BP_PlayerController.BP_PlayerController_C"));
    TDLegacy::SetObject(PC,TEXT("GameMode"),Native);
    for (uint8 Rarity=0; Rarity<5; ++Rarity)
    {
        UFunction* F=PC->FindFunction(TEXT("UpgradeTowerRank")); check(F);
        FStructOnScope Args(F);
        FindFProperty<FByteProperty>(F,TEXT("PreviousTower"))->SetPropertyValue_InContainer(Args.GetStructMemory(),Rarity);
        PC->ProcessEvent(F,Args.GetStructMemory());
        const uint8 Expected=FindFProperty<FByteProperty>(F,TEXT("UpgradeTower"))->GetPropertyValue_InContainer(Args.GetStructMemory());
        uint8 Actual=255; UTDGameplayLibrary::UpgradeTowerRank(PC,Rarity,Actual);
        Expect(Actual==Expected,FString::Printf(TEXT("Upgrade rank %d parity"),Rarity));
    }
    UTDGameplayLibrary::BeginMergeMode(PC);
    Expect(Read(PC,TEXT("PlayerMode"))==2,TEXT("Merge press mode"));
    UTDGameplayLibrary::EndTowerMode(PC);
    Expect(Read(PC,TEXT("PlayerMode"))==0,TEXT("Merge release mode"));
    UTDGameplayLibrary::BeginSellMode(PC);
    Expect(Read(PC,TEXT("PlayerMode"))==3,TEXT("Sell press mode"));
    UTDGameplayLibrary::ToggleBuildMode(PC);
    Expect(Read(PC,TEXT("PlayerMode"))==3,TEXT("Build toggle does not override held sell mode"));
    UTDGameplayLibrary::EndTowerMode(PC);
    UTDGameplayLibrary::ToggleBuildMode(PC);
    Expect(Read(PC,TEXT("PlayerMode"))==1 && IsValid(TDLegacy::Object(PC,TEXT("CurrentGhostTower"))),TEXT("Build toggle creates preview"));
    AActor* Slot=Spawn(TEXT("/Game/Blueprint/MapStructure/BP_TowerSlot.BP_TowerSlot_C"));
    Slot->SetActorLocation(FVector(10000,10000,5));
    Write(Native,TEXT("Gold"),50);
    const double Cost=Read(Native,TEXT("NormalTowerCost"));
    UTDGameplayLibrary::ControllerInteract(PC,Slot,true);
    Expect(Read(Native,TEXT("Gold"))==50-Cost && TDLegacy::Bool(Slot,TEXT("bIsOccupied")),TEXT("Building charges cost and occupies slot"));
    UTDGameplayLibrary::ControllerInteract(PC,Slot,true);
    Expect(Read(Native,TEXT("Gold"))==50-Cost,TEXT("Occupied slot does not charge again"));
    TDLegacy::SetBool(Slot,TEXT("bIsOccupied"),false);
    Write(Native,TEXT("Gold"),0);
    UTDGameplayLibrary::ControllerInteract(PC,Slot,true);
    Expect(Read(Native,TEXT("Gold"))==0 && !TDLegacy::Bool(Slot,TEXT("bIsOccupied")),TEXT("Insufficient gold leaves slot free"));
    UTDGameplayLibrary::ToggleBuildMode(PC);
    Expect(Read(PC,TEXT("PlayerMode"))==0 && !IsValid(TDLegacy::Object(PC,TEXT("CurrentGhostTower"))),TEXT("Build toggle removes preview"));
    UE_LOG(LogTemp,Display,TEXT("TD_VERIFY checks=%d failures=%d; no content saved"),Checks,Failures);
    World->DestroyWorld(false);
    return Failures ? 1 : 0;
}
