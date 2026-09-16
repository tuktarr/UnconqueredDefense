#include "Character/Monster/AI/TDAIRules.h"
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
void FTDAIRules::MonsterPossess(UObject* Context, APawn* PossessedPawn)
{
    auto* AI = Cast<AAIController>(Context);
    if (!AI) return;
    UBlackboardComponent* Blackboard = nullptr;
    AI->UseBlackboard(LoadObject<UBlackboardData>(nullptr, TEXT("/Game/Blueprint/Actor/Monster/AI/BB_Monster.BB_Monster")), Blackboard);
    AI->RunBehaviorTree(LoadObject<UBehaviorTree>(nullptr, TEXT("/Game/Blueprint/Actor/Monster/AI/BT_Monster.BT_Monster")));
    if (Blackboard) Blackboard->SetValueAsBool(Name(Context, TEXT("IsAliveKey")), true);
}
static FBlackboardKeySelector Key(UObject* Context, FName Field)
{
    auto* P = CastField<FStructProperty>(Property(Context, Field));
    return P && P->Struct == FBlackboardKeySelector::StaticStruct() ? *P->ContainerPtrToValuePtr<FBlackboardKeySelector>(Context) : FBlackboardKeySelector();
}
void FTDAIRules::ClearAITarget(UObject* Context, AAIController* OwnerController, APawn* ControlledPawn)
{
    UBTFunctionLibrary::SetBlackboardValueAsObject(Cast<UBTNode>(Context), Key(Context, TEXT("TargetActorKey")), nullptr);
    if (auto* Task = Cast<UBTTask_BlueprintBase>(Context))
        if (UFunction* F = Task->FindFunction(TEXT("FinishExecute")))
        {
            struct { bool bSuccess; } Params{true};
            Task->ProcessEvent(F, &Params);
        }
}
void FTDAIRules::FindNextCheckpoint(UObject* Context, AAIController* OwnerController, APawn* ControlledPawn, float DeltaSeconds)
{
    auto* Node = Cast<UBTNode>(Context);
    if (!Node || !IsMonster(ControlledPawn)) return;
    const FBlackboardKeySelector TargetKey = Key(Context, TEXT("TargetActorKey"));
    if (IsValid(UBTFunctionLibrary::GetBlackboardValueAsActor(Node, TargetKey))) return;
    const FBlackboardKeySelector IndexKey = Key(Context, TEXT("CurrentIndexKey"));
    const int32 Index = UBTFunctionLibrary::GetBlackboardValueAsInt(Node, IndexKey);
    const TArray<UObject*> Points = Objects(ControlledPawn, TEXT("WayPoint"));
    UBTFunctionLibrary::SetBlackboardValueAsObject(Node, TargetKey, Points.IsValidIndex(Index) ? Points[Index] : nullptr);
    UBTFunctionLibrary::SetBlackboardValueAsInt(Node, IndexKey, (Index + 1) % 4);
}
