#include "Character/Monster/TDMonsterRules.h"
#include "Components/Health/TDHealthRules.h"
#include "GameMode/Economy/TDEconomyRules.h"
#include "GameMode/Rounds/TDRoundRules.h"
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
FVector Vector(UObject* O,FName N)
{
    auto* P=CastField<FStructProperty>(Property(O,N));
    return P && P->Struct==TBaseStructure<FVector>::Get() ? *P->ContainerPtrToValuePtr<FVector>(O) : FVector::ZeroVector;
}
}
void FTDMonsterRules::MonsterActivate(UObject* Context)
{
    auto* Monster=Cast<ACharacter>(Context);
    if (!Monster) return;
    FTDRoundRules::AddMonsterCount(UGameplayStatics::GetGameMode(Context));
    Monster->SetActorHiddenInGame(false);
    FRoundData Row(Object(Context,TEXT("MonsterData")));
    Monster->GetMesh()->SetSkeletalMeshAsset(Cast<USkeletalMesh>(Row.Object(TEXT("MonsterMesh"))));
    Monster->GetMesh()->SetAnimInstanceClass(Cast<UClass>(Row.Object(TEXT("MonsterABP"))));
    Monster->GetMesh()->SetVisibility(true);
    SetBool(Context,TEXT("IsActive"),true);
    Monster->SetActorEnableCollision(true);
    MonsterSpawnFromPool(Context);
    Monster->SetActorLocation(Vector(Context,TEXT("Spawn Location")),false,nullptr,ETeleportType::TeleportPhysics);
}
void FTDMonsterRules::MonsterSpawnFromPool(UObject* Context)
{
    FRoundData Row(Object(Context,TEXT("MonsterData")));
    UObject* Health=Object(Context,TEXT("HP_Controller"));
    const double HP=Row.Number(TEXT("MaxHp"));
    SetNumber(Health,TEXT("MaxHP"),HP);
    SetNumber(Context,TEXT("RewardGold"),Row.Number(TEXT("RewardGold")));
    FTDHealthRules::InitializeHealth(Health);
    if (UObject* Widget=Object(Context,TEXT("MonsterHPBar")))
        if (UFunction* F=Widget->FindFunction(TEXT("UpdateHp")))
        {
            FStructOnScope Args(F);
            for (const FName N : {FName(TEXT("CurrentHp")),FName(TEXT("MaxHp"))})
                if (auto* P=FindFProperty<FDoubleProperty>(F,N)) P->SetPropertyValue_InContainer(Args.GetStructMemory(),HP);
            Widget->ProcessEvent(F,Args.GetStructMemory());
        }
    if (auto* Actor=Cast<AActor>(Context)) Actor->SetActorScale3D(FVector(Row.Bool(TEXT("IsBoss")) ? 3 : 1));
}
void FTDMonsterRules::MonsterDeactivate(UObject* Context)
{
    auto* Monster=Cast<APawn>(Context);
    if (!Monster) return;
    if (auto* AI=Cast<AAIController>(Monster->GetController()))
    {
        AI->StopMovement();
        if (auto* BB=AI->GetBlackboardComponent())
        {
            BB->ClearValue(Name(Context,TEXT("TargetActor")));
            BB->SetValueAsInt(Name(Context,TEXT("CurrentIndex")),0);
        }
    }
    SetBool(Context,TEXT("IsActive"),false);
    Monster->SetActorEnableCollision(false);
    Monster->SetActorHiddenInGame(true);
    Monster->SetActorLocation(FVector(0,0,-100),false,nullptr,ETeleportType::TeleportPhysics);
    FRoundData Row(Object(Context,TEXT("MonsterData")));
    if (Row.Bool(TEXT("IsBoss"))) Broadcast(UGameplayStatics::GetGameMode(Context),TEXT("OnBossDied"));
}
void FTDMonsterRules::MonsterDamage(UObject* Context,float Damage)
{
    if (Bool(Context,TEXT("IsActive"))) FTDHealthRules::ReceiveDamage(Object(Context,TEXT("HP_Controller")),Damage);
}
void FTDMonsterRules::MonsterReward(UObject* Context)
{
    UObject* GameMode=UGameplayStatics::GetGameMode(Context);
    FTDEconomyRules::AddGold(GameMode,Number(Context,TEXT("RewardGold")));
    FTDRoundRules::MinusMonsterCount(GameMode);
}
void FTDMonsterRules::MonsterRoundChanged(UObject* Context)
{
    SetNumber(Context,TEXT("CurrentRound"),Number(UGameplayStatics::GetGameMode(Context),TEXT("CurrentRound")));
}
