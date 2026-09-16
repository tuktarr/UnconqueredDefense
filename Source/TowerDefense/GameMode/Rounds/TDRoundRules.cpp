#include "GameMode/Rounds/TDRoundRules.h"
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
void FTDRoundRules::AddMonsterCount(UObject* Context)
{
    SetNumber(Context, TEXT("MonsterCount"), Number(Context, TEXT("MonsterCount")) + 1);
    Broadcast(Context, TEXT("OnMonsterNumChanged"));
}
void FTDRoundRules::MinusMonsterCount(UObject* Context)
{
    SetNumber(Context, TEXT("MonsterCount"), Number(Context, TEXT("MonsterCount")) - 1);
    Broadcast(Context, TEXT("OnMonsterNumChanged"));
}
void FTDRoundRules::GoNextRound(UObject* Context)
{
    SetNumber(Context, TEXT("CurrentRound"), Number(Context, TEXT("CurrentRound")) + 1);
    SetNumber(Context, TEXT("RemainingTime"), 30.0);
    if (Number(Context, TEXT("CurrentRound")) > Number(Context, TEXT("MaxRound")))
        Call(Context, Number(Context, TEXT("MonsterCount")) > 0 ? TEXT("GameOver") : TEXT("GameClear"));
    else Broadcast(Context, TEXT("OnRoundChanged"));
}
void FTDRoundRules::UpdateRoundData(UObject* Context)
{
    UDataTable* Table = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprint/Data/DT_RoundData.DT_RoundData"));
    auto* P = CastField<FStructProperty>(Property(Context, TEXT("CurrentRoundData")));
    if (!Table || !P || Table->GetRowStruct() != P->Struct) return;
    const uint8* Row = Table->FindRowUnchecked(FName(*FString::Printf(TEXT("Monster_%d"), static_cast<int32>(Number(Context, TEXT("CurrentRound"))))));
    if (Row) P->Struct->CopyScriptStruct(P->ContainerPtrToValuePtr<void>(Context), Row);
}
void FTDRoundRules::GameModeTick(UObject* Context, float DeltaSeconds)
{
    SetNumber(Context, TEXT("RemainingTime"), Number(Context, TEXT("RemainingTime")) - DeltaSeconds);
    Broadcast(Context, TEXT("OnTimerUpdated"));
    UpdateRoundData(Context);
    if (Number(Context, TEXT("MonsterCount")) >= Number(Context, TEXT("MaxMonsterCount")))
    {
        Call(Context, TEXT("GameOver"));
        return;
    }
    if (Number(Context, TEXT("RemainingTime")) > 0) return;
    bool IsBoss = false;
    if (auto* P = CastField<FStructProperty>(Property(Context, TEXT("CurrentRoundData"))))
        for (TFieldIterator<FBoolProperty> It(P->Struct); It; ++It)
            if (It->GetName().StartsWith(TEXT("IsBoss_"))) IsBoss = It->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Context));
    if (IsBoss && Number(Context, TEXT("MonsterCount")) > 0) Call(Context, TEXT("GameOver"));
    else GoNextRound(Context);
}
