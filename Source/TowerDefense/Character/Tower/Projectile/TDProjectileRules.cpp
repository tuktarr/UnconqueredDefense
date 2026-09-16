#include "Character/Tower/Projectile/TDProjectileRules.h"
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
void FTDProjectileRules::ProjectileOverlap(UObject* Context, AActor* OtherActor)
{
    AActor* Shot = Cast<AActor>(Context);
    if (Shot && IsMonster(OtherActor))
    {
        UGameplayStatics::ApplyDamage(OtherActor, Number(Context, TEXT("ProjectileDamage")), nullptr, Shot->GetOwner(), nullptr);
        Shot->Destroy();
    }
}
void FTDProjectileRules::CannonOverlap(UObject* Context, AActor* OtherActor)
{
    AActor* Shot = Cast<AActor>(Context);
    if (Shot && IsMonster(OtherActor))
    {
        UGameplayStatics::ApplyRadialDamage(Context, Number(Context, TEXT("ProjectileDamage")), OtherActor->GetActorLocation(), 2000.0f, nullptr, {}, Shot->GetOwner(), nullptr, true);
        Shot->Destroy();
    }
}
static void SetProjectileActive(UObject* Context, bool Active)
{
    if (AActor* Shot = Cast<AActor>(Context))
    {
        SetBool(Context, TEXT("bIsActive"), Active);
        Shot->SetActorHiddenInGame(!Active);
        Shot->SetActorEnableCollision(Active);
        if (auto* Movement = Cast<UProjectileMovementComponent>(Object(Context, TEXT("ProjectileMovement"))))
        {
            if (Active) Movement->Activate(); else Movement->Deactivate();
        }
    }
}
void FTDProjectileRules::ActivateProjectile(UObject* Context) { SetProjectileActive(Context, true); }
void FTDProjectileRules::DeactivateProjectile(UObject* Context) { SetProjectileActive(Context, false); }
