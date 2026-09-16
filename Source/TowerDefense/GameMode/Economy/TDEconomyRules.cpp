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
void FTDEconomyRules::SpendGold(UObject* Context, double Amount, bool& Success)
{
    Success = Number(Context, TEXT("Gold")) >= Amount;
    if (Success)
    {
        SetNumber(Context, TEXT("Gold"), Number(Context, TEXT("Gold")) - Amount);
        Broadcast(Context, TEXT("OnGoldChanged"));
    }
}
void FTDEconomyRules::AddGold(UObject* Context, double Amount)
{
    SetNumber(Context, TEXT("Gold"), Number(Context, TEXT("Gold")) + Amount);
    Broadcast(Context, TEXT("OnGoldChanged"));
}
