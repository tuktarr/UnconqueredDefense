#include "Character/Tower/Combat/TDTowerAttackRules.h"
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
static AActor* FirstTarget(UObject* Context, bool RemoveInvalid)
{
    const TArray<UObject*> Targets = Objects(Context, TEXT("TargetArray"));
    AActor* Target = Targets.IsEmpty() ? nullptr : Cast<AActor>(Targets[0]);
    if (IsValid(Target)) return Target;
    if (RemoveInvalid) RemoveFirst(Context, TEXT("TargetArray"));
    return nullptr;
}
void FTDTowerAttackRules::SwordAction(UObject* Context)
{
    if (AActor* Target = FirstTarget(Context, true)) UGameplayStatics::ApplyDamage(Target, Number(Context, TEXT("Damage")), nullptr, nullptr, nullptr);
}
static AActor* SpawnShot(UObject* Context, AActor* Target, const TCHAR* ClassPath, FName Socket, bool AlwaysSpawn)
{
    AActor* Tower = Cast<AActor>(Context);
    auto* Mesh = Cast<USkeletalMeshComponent>(Object(Context, TEXT("SkeletalMesh")));
    UClass* Class = LoadClass<AActor>(nullptr, ClassPath);
    if (!Tower || !Mesh || !Class) return nullptr;
    const FVector Location = Mesh->GetSocketLocation(Socket);
    const FTransform Transform(FRotator(0, (Target->GetActorLocation()-Location).Rotation().Yaw, 0), Location);
    AActor* Shot = Tower->GetWorld()->SpawnActorDeferred<AActor>(Class, Transform, nullptr, nullptr,
        AlwaysSpawn ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::Undefined);
    if (Shot)
    {
        SetNumber(Shot, TEXT("ProjectileDamage"), Number(Context, TEXT("Damage")));
        UGameplayStatics::FinishSpawningActor(Shot, Transform);
    }
    return Shot;
}
void FTDTowerAttackRules::GunAction(UObject* Context)
{
    if (AActor* Target = FirstTarget(Context, true))
        if (AActor* Shot = SpawnShot(Context, Target, TEXT("/Game/Blueprint/Actor/Tower/Projectile/BP_Projectile_Bullet.BP_Projectile_Bullet_C"), TEXT("GunOutput"), false))
            if (auto* Movement = Cast<UProjectileMovementComponent>(Object(Shot, TEXT("ProjectileMovement")))) Movement->HomingTargetComponent = Target->GetRootComponent();
}
void FTDTowerAttackRules::RobotAction(UObject* Context)
{
    if (AActor* Target = FirstTarget(Context, true))
    {
        SetVector(Context, TEXT("TargetLocation"), Target->GetActorLocation());
        const TCHAR* Path = TEXT("/Game/Blueprint/Actor/Tower/Projectile/BP_Projectile_CannonBall.BP_Projectile_CannonBall_C");
        SpawnShot(Context, Target, Path, TEXT("CannonBarrel_L"), true);
        SpawnShot(Context, Target, Path, TEXT("CannonBarrel_R"), true);
    }
}
void FTDTowerAttackRules::MagicAction(UObject* Context)
{
    if (AActor* Target = FirstTarget(Context, false))
    {
        SetVector(Context, TEXT("TargetLocation"), Target->GetActorLocation());
        // The original Niagara template is null, so only radial damage has an effect.
        UGameplayStatics::ApplyRadialDamage(Context, Number(Context, TEXT("Damage")), Target->GetActorLocation(), Number(Context, TEXT("SpellRadius")), nullptr, {}, nullptr, nullptr, true);
    }
}
void FTDTowerAttackRules::SupportAction(UObject* Context)
{
    FTDEconomyRules::AddGold(UGameplayStatics::GetGameMode(Context), Number(Context, TEXT("TurnGold")));
}
