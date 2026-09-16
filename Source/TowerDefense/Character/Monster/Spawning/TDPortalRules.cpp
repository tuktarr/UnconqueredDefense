#include "Character/Monster/Spawning/TDPortalRules.h"
#include "Character/Monster/TDMonsterRules.h"
#include "BlueprintBridge/TDLegacyData.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

using namespace TDLegacy;
namespace
{
void AppendObject(UObject* O,FName N,UObject* Value)
{
    auto* P=CastField<FArrayProperty>(Property(O,N));
    auto* Inner=P ? CastField<FObjectPropertyBase>(P->Inner) : nullptr;
    if (!Inner) return;
    FScriptArrayHelper Array(P,P->ContainerPtrToValuePtr<void>(O));
    Inner->SetObjectPropertyValue(Array.GetRawPtr(Array.AddValue()),Value);
}
FTimerHandle* Timer(UObject* O,FName N)
{
    auto* P=CastField<FStructProperty>(Property(O,N));
    return P && P->Struct==FTimerHandle::StaticStruct() ? P->ContainerPtrToValuePtr<FTimerHandle>(O) : nullptr;
}
void BindEvent(UObject* O,FName Event,UObject* Target,FName Handler)
{
    if (auto* P=CastField<FMulticastDelegateProperty>(Property(O,Event)))
    {
        FScriptDelegate D; D.BindUFunction(Target,Handler); P->AddDelegate(D,O);
    }
}
void StartTimer(UObject* O,FName Field,FName Handler,float Period,float Delay)
{
    if (FTimerHandle* Handle=Timer(O,Field))
    {
        FTimerDynamicDelegate D; D.BindUFunction(O,Handler);
        *Handle=UKismetSystemLibrary::K2_SetTimerDelegate(D,Period,true,false,Delay,0);
    }
}
}
void FTDPortalRules::PortalBeginPlay(UObject* Context)
{
    if (auto* P=CastField<FArrayProperty>(Property(Context,TEXT("WaypointList"))))
        FScriptArrayHelper(P,P->ContainerPtrToValuePtr<void>(Context)).EmptyValues();
    UClass* Waypoint=LoadClass<AActor>(nullptr,TEXT("/Game/Blueprint/MapStructure/BP_Waypoint.BP_Waypoint_C"));
    for (int32 I=0; I<Number(Context,TEXT("WaypointCount")); ++I)
    {
        TArray<AActor*> Points;
        UGameplayStatics::GetAllActorsWithTag(Context,FName(*FString::Printf(TEXT("Game.WayPoint%d"),I+1)),Points);
        if (!Points.IsEmpty() && Waypoint && Points[0]->IsA(Waypoint)) AppendObject(Context,TEXT("WaypointList"),Points[0]);
    }
    StartTimer(Context,TEXT("PoolFillTimerHandle"),TEXT("CreateMonster"),0.01f,0);
    StartTimer(Context,TEXT("SpawnTimerHandle"),TEXT("SpawnMonster"),1.0f,0.5f);
    UObject* GameMode=UGameplayStatics::GetGameMode(Context);
    BindEvent(GameMode,TEXT("OnBossDied"),Context,TEXT("ResumeSpawning"));
    BindEvent(GameMode,TEXT("OnRoundChanged"),Context,TEXT("SetBossFlag"));
}
void FTDPortalRules::PortalCreateMonster(UObject* Context)
{
    if (!Context || !Context->GetWorld()) return;
    const int32 Count=Number(Context,TEXT("CurrentSpawnCount"))+1;
    SetNumber(Context,TEXT("CurrentSpawnCount"),Count);
    // Keep the original pre-increment and strict comparison (PoolSize-1 actors).
    if (Count>=Number(Context,TEXT("PoolSize")))
    {
        if (auto* Handle=Timer(Context,TEXT("PoolFillTimerHandle"))) Context->GetWorld()->GetTimerManager().ClearTimer(*Handle);
        return;
    }
    UClass* Class=LoadClass<APawn>(nullptr,TEXT("/Game/Blueprint/Actor/Monster/BP_Monster.BP_Monster_C"));
    if (!Class) return;
    const FTransform Transform(FRotator(0,-90,0),FVector(0,0,FMath::FRandRange(10.0,30.0)));
    APawn* Monster=Context->GetWorld()->SpawnActorDeferred<APawn>(Class,Transform);
    if (!Monster) return;
    UGameplayStatics::FinishSpawningActor(Monster,Transform);
    Monster->SpawnDefaultController();
    FTDMonsterRules::MonsterDeactivate(Monster);
    auto* From=CastField<FArrayProperty>(Property(Context,TEXT("WaypointList")));
    auto* To=CastField<FArrayProperty>(Property(Monster,TEXT("WayPoint")));
    if (From && To && From->SameType(To)) To->CopyCompleteValue(To->ContainerPtrToValuePtr<void>(Monster),From->ContainerPtrToValuePtr<void>(Context));
    AppendObject(Context,TEXT("MonsterPool"),Monster);
}
void FTDPortalRules::PortalSpawnMonster(UObject* Context)
{
    for (UObject* Monster : Objects(Context,TEXT("MonsterPool")))
    {
        if (!IsValid(Monster) || Bool(Monster,TEXT("IsActive"))) continue;
        FRoundData Row(Object(Monster,TEXT("MonsterData")));
        if (!Row.Bool(TEXT("IsBoss"))) { FTDMonsterRules::MonsterActivate(Monster); break; }
        if (Bool(Context,TEXT("bHasSpawnBoss"))) break;
        SetBool(Context,TEXT("bHasSpawnBoss"),true);
        FTDMonsterRules::MonsterActivate(Monster);
        if (auto* Handle=Timer(Context,TEXT("SpawnTimerHandle"))) Context->GetWorld()->GetTimerManager().PauseTimer(*Handle);
        if (Row.Number(TEXT("RoundCount"))==Number(Context,TEXT("FinalRound"))) Broadcast(UGameplayStatics::GetGameMode(Context),TEXT("OnBossSpawned"));
        break;
    }
}
void FTDPortalRules::PortalResumeSpawning(UObject* Context)
{
    if (Context && Context->GetWorld()) if (auto* Handle=Timer(Context,TEXT("SpawnTimerHandle"))) Context->GetWorld()->GetTimerManager().UnPauseTimer(*Handle);
}
void FTDPortalRules::PortalResetBoss(UObject* Context) { SetBool(Context,TEXT("bHasSpawnBoss"),false); }
