#include "Character/Player/TDCameraRules.h"
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
void FTDCameraRules::MoveToMouseDirection(UObject* Context, double PosX, double PosY, double& PosX_2, double& PosY_2)
{
    const FVector2D Size = UWidgetLayoutLibrary::GetViewportSize(Context);
    const double Edge = Number(Context, TEXT("EdgeThreshold"));
    const double Speed = Number(Context, TEXT("ScrollSpeed"));
    PosX_2 = ((PosX < Edge ? -1.0 : 0.0) + (PosX > Size.X - 200.0 - Edge ? 1.0 : 0.0)) * Speed;
    PosY_2 = ((PosY < Edge ? 1.0 : 0.0) + (PosY > Size.Y - 200.0 - Edge ? -1.0 : 0.0)) * Speed;
}
void FTDCameraRules::PlayerTick(UObject* Context, float DeltaSeconds)
{
    APawn* Pawn = Cast<APawn>(Context);
    if (!Pawn) return;
    const FVector2D Mouse = UWidgetLayoutLibrary::GetMousePositionOnViewport(Context);
    double X, Y;
    MoveToMouseDirection(Context, Mouse.X, Mouse.Y, X, Y);
    Pawn->AddMovementInput(Pawn->GetActorRightVector(), X);
    Pawn->AddMovementInput(Pawn->GetActorForwardVector(), Y);
}
void FTDCameraRules::PlayerZoom(UObject* Context, float ActionValue)
{
    if (auto* Arm = Cast<USpringArmComponent>(Object(Context, TEXT("SpringArm"))))
        Arm->TargetArmLength = FMath::Clamp(Arm->TargetArmLength + ActionValue * 100.0f, 700.0f, 4000.0f);
}
