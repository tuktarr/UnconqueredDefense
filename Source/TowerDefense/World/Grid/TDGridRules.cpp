#include "World/Grid/TDGridRules.h"
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
void FTDGridRules::GenerateGrid(UObject* Context)
{
    if (!Context || !Context->GetWorld()) return;
    UClass* SlotClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blueprint/MapStructure/BP_TowerSlot.BP_TowerSlot_C"));
    if (!SlotClass) return;
    const int32 XCount = Number(Context, TEXT("GridSizeX")), YCount = Number(Context, TEXT("GridSizeY"));
    const int32 Spacing = Number(Context, TEXT("TileSpacing"));
    for (int32 X=0; X<XCount; ++X) for (int32 Y=0; Y<YCount; ++Y)
    {
        const FTransform Transform(FRotator::ZeroRotator, FVector(X*Spacing-1540, Y*Spacing-2050, 5), FVector(2,2,1));
        AActor* Slot = Context->GetWorld()->SpawnActorDeferred<AActor>(SlotClass, Transform);
        if (Slot) UGameplayStatics::FinishSpawningActor(Slot, Transform);
    }
}
