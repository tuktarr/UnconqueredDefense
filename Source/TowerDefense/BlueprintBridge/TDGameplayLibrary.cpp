#include "BlueprintBridge/TDGameplayLibrary.h"
#include "Character/Monster/TDMonsterRules.h"
#include "Character/Monster/AI/TDAIRules.h"
#include "Character/Monster/Spawning/TDPortalRules.h"
#include "Character/Player/TDCameraRules.h"
#include "Character/Player/TDPlayerRules.h"
#include "Character/Tower/TDTowerRules.h"
#include "Character/Tower/Combat/TDTowerAttackRules.h"
#include "Character/Tower/Projectile/TDProjectileRules.h"
#include "Components/Health/TDHealthRules.h"
#include "GameMode/Economy/TDEconomyRules.h"
#include "GameMode/Rounds/TDRoundRules.h"
#include "World/Grid/TDGridRules.h"

#include "Data/TDDataRules.h"
#include "UObject/UnrealType.h"

// Compatibility entry points for existing serialized Blueprint calls.
void UTDGameplayLibrary::MonsterActivate(UObject* Context)
{
    FTDMonsterRules::MonsterActivate(Context);
}

void UTDGameplayLibrary::MonsterSpawnFromPool(UObject* Context)
{
    FTDMonsterRules::MonsterSpawnFromPool(Context);
}

void UTDGameplayLibrary::MonsterDeactivate(UObject* Context)
{
    FTDMonsterRules::MonsterDeactivate(Context);
}

void UTDGameplayLibrary::MonsterDamage(UObject* Context,float Damage)
{
    FTDMonsterRules::MonsterDamage(Context, Damage);
}

void UTDGameplayLibrary::MonsterReward(UObject* Context)
{
    FTDMonsterRules::MonsterReward(Context);
}

void UTDGameplayLibrary::MonsterRoundChanged(UObject* Context)
{
    FTDMonsterRules::MonsterRoundChanged(Context);
}

void UTDGameplayLibrary::MonsterPossess(UObject* Context, APawn* PossessedPawn)
{
    FTDAIRules::MonsterPossess(Context, PossessedPawn);
}

void UTDGameplayLibrary::ClearAITarget(UObject* Context, AAIController* OwnerController, APawn* ControlledPawn)
{
    FTDAIRules::ClearAITarget(Context, OwnerController, ControlledPawn);
}

void UTDGameplayLibrary::FindNextCheckpoint(UObject* Context, AAIController* OwnerController, APawn* ControlledPawn, float DeltaSeconds)
{
    FTDAIRules::FindNextCheckpoint(Context, OwnerController, ControlledPawn, DeltaSeconds);
}

void UTDGameplayLibrary::PortalBeginPlay(UObject* Context)
{
    FTDPortalRules::PortalBeginPlay(Context);
}

void UTDGameplayLibrary::PortalCreateMonster(UObject* Context)
{
    FTDPortalRules::PortalCreateMonster(Context);
}

void UTDGameplayLibrary::PortalSpawnMonster(UObject* Context)
{
    FTDPortalRules::PortalSpawnMonster(Context);
}

void UTDGameplayLibrary::PortalResumeSpawning(UObject* Context)
{
    FTDPortalRules::PortalResumeSpawning(Context);
}

void UTDGameplayLibrary::PortalResetBoss(UObject* Context)
{
    FTDPortalRules::PortalResetBoss(Context);
}

void UTDGameplayLibrary::MoveToMouseDirection(UObject* Context, double PosX, double PosY, double& PosX_2, double& PosY_2)
{
    FTDCameraRules::MoveToMouseDirection(Context, PosX, PosY, PosX_2, PosY_2);
}

void UTDGameplayLibrary::PlayerTick(UObject* Context, float DeltaSeconds)
{
    FTDCameraRules::PlayerTick(Context, DeltaSeconds);
}

void UTDGameplayLibrary::PlayerZoom(UObject* Context, float ActionValue)
{
    FTDCameraRules::PlayerZoom(Context, ActionValue);
}

void UTDGameplayLibrary::UpgradeTowerRank(UObject* Context,uint8 PreviousTower,uint8& UpgradeTower)
{
    FTDPlayerRules::UpgradeTowerRank(Context, PreviousTower, UpgradeTower);
}

void UTDGameplayLibrary::ToggleBuildMode(UObject* Context)
{
    FTDPlayerRules::ToggleBuildMode(Context);
}

void UTDGameplayLibrary::BeginMergeMode(UObject* Context)
{
    FTDPlayerRules::BeginMergeMode(Context);
}

void UTDGameplayLibrary::BeginSellMode(UObject* Context)
{
    FTDPlayerRules::BeginSellMode(Context);
}

void UTDGameplayLibrary::EndTowerMode(UObject* Context)
{
    FTDPlayerRules::EndTowerMode(Context);
}

void UTDGameplayLibrary::ControllerTick(UObject* Context)
{
    FTDPlayerRules::ControllerTick(Context);
}

void UTDGameplayLibrary::UpdateGhostTowerDisplay(UObject* Context)
{
    FTDPlayerRules::UpdateGhostTowerDisplay(Context);
}

void UTDGameplayLibrary::ControllerClick(UObject* Context)
{
    FTDPlayerRules::ControllerClick(Context);
}

void UTDGameplayLibrary::ControllerInteract(UObject* Context,AActor* Actor,bool HitSomething)
{
    FTDPlayerRules::ControllerInteract(Context, Actor, HitSomething);
}

void UTDGameplayLibrary::ClearGhostTower(UObject* Context)
{
    FTDPlayerRules::ClearGhostTower(Context);
}

void UTDGameplayLibrary::TowerBeginOverlap(UObject* Context, AActor* OtherActor)
{
    FTDTowerRules::TowerBeginOverlap(Context, OtherActor);
}

void UTDGameplayLibrary::TowerEndOverlap(UObject* Context, AActor* OtherActor)
{
    FTDTowerRules::TowerEndOverlap(Context, OtherActor);
}

void UTDGameplayLibrary::TowerAttackNotify(UObject* Context)
{
    FTDTowerRules::TowerAttackNotify(Context);
}

void UTDGameplayLibrary::TowerAttackEndNotify(UObject* Context)
{
    FTDTowerRules::TowerAttackEndNotify(Context);
}

void UTDGameplayLibrary::DestroyAndFreeSlot(UObject* Context)
{
    FTDTowerRules::DestroyAndFreeSlot(Context);
}

void UTDGameplayLibrary::TowerBossSpawned(UObject* Context)
{
    FTDTowerRules::TowerBossSpawned(Context);
}

void UTDGameplayLibrary::TowerSold(UObject* Context,double Amount)
{
    FTDTowerRules::TowerSold(Context, Amount);
}

void UTDGameplayLibrary::SwordAction(UObject* Context)
{
    FTDTowerAttackRules::SwordAction(Context);
}

void UTDGameplayLibrary::GunAction(UObject* Context)
{
    FTDTowerAttackRules::GunAction(Context);
}

void UTDGameplayLibrary::RobotAction(UObject* Context)
{
    FTDTowerAttackRules::RobotAction(Context);
}

void UTDGameplayLibrary::MagicAction(UObject* Context)
{
    FTDTowerAttackRules::MagicAction(Context);
}

void UTDGameplayLibrary::SupportAction(UObject* Context)
{
    FTDTowerAttackRules::SupportAction(Context);
}

void UTDGameplayLibrary::ProjectileOverlap(UObject* Context, AActor* OtherActor)
{
    FTDProjectileRules::ProjectileOverlap(Context, OtherActor);
}

void UTDGameplayLibrary::CannonOverlap(UObject* Context, AActor* OtherActor)
{
    FTDProjectileRules::CannonOverlap(Context, OtherActor);
}

void UTDGameplayLibrary::ActivateProjectile(UObject* Context)
{
    FTDProjectileRules::ActivateProjectile(Context);
}

void UTDGameplayLibrary::DeactivateProjectile(UObject* Context)
{
    FTDProjectileRules::DeactivateProjectile(Context);
}

void UTDGameplayLibrary::InitializeHealth(UObject* Context)
{
    FTDHealthRules::InitializeHealth(Context);
}

void UTDGameplayLibrary::UpdateMaxHealth(UObject* Context, double MaxHP)
{
    FTDHealthRules::UpdateMaxHealth(Context, MaxHP);
}

void UTDGameplayLibrary::ReceiveDamage(UObject* Context, double Damage)
{
    FTDHealthRules::ReceiveDamage(Context, Damage);
}

void UTDGameplayLibrary::SpendGold(UObject* Context, double Amount, bool& Success)
{
    FTDEconomyRules::SpendGold(Context, Amount, Success);
}

void UTDGameplayLibrary::AddGold(UObject* Context, double Amount)
{
    FTDEconomyRules::AddGold(Context, Amount);
}

void UTDGameplayLibrary::AddMonsterCount(UObject* Context)
{
    FTDRoundRules::AddMonsterCount(Context);
}

void UTDGameplayLibrary::MinusMonsterCount(UObject* Context)
{
    FTDRoundRules::MinusMonsterCount(Context);
}

void UTDGameplayLibrary::GoNextRound(UObject* Context)
{
    FTDRoundRules::GoNextRound(Context);
}

void UTDGameplayLibrary::UpdateRoundData(UObject* Context)
{
    FTDRoundRules::UpdateRoundData(Context);
}

void UTDGameplayLibrary::GameModeTick(UObject* Context, float DeltaSeconds)
{
    FTDRoundRules::GameModeTick(Context, DeltaSeconds);
}

void UTDGameplayLibrary::GenerateGrid(UObject* Context)
{
    FTDGridRules::GenerateGrid(Context);
}

// Custom thunks preserve the existing Blueprint struct signatures; data assets
// and split-struct consumers do not have to be rewritten or lose serialized data.
void UTDGameplayLibrary::GetCurrentRoundData(UObject*, int32&) { checkNoEntry(); }
DEFINE_FUNCTION(UTDGameplayLibrary::execGetCurrentRoundData)
{
    P_GET_OBJECT(UObject,LegacyContext);
    Stack.StepCompiledIn<FStructProperty>(nullptr);
    auto* P=CastField<FStructProperty>(Stack.MostRecentProperty);
    void* Address=Stack.MostRecentPropertyAddress;
    P_FINISH;
    P_NATIVE_BEGIN;
    FTDDataRules::GetCurrentRoundData(LegacyContext,P,Address);
    P_NATIVE_END;
}
void UTDGameplayLibrary::InitializeTower(UObject*, FName, int32&) { checkNoEntry(); }
DEFINE_FUNCTION(UTDGameplayLibrary::execInitializeTower)
{
    P_GET_OBJECT(UObject,LegacyContext);
    P_GET_PROPERTY(FNameProperty,RowName);
    Stack.StepCompiledIn<FStructProperty>(nullptr);
    auto* Output=CastField<FStructProperty>(Stack.MostRecentProperty);
    void* Address=Stack.MostRecentPropertyAddress;
    P_FINISH;
    P_NATIVE_BEGIN;
    FTDDataRules::InitializeTower(LegacyContext,RowName,Output,Address);
    P_NATIVE_END;
}
void UTDGameplayLibrary::GetRandomTowerByRarity(UObject*, uint8, int32&) { checkNoEntry(); }
DEFINE_FUNCTION(UTDGameplayLibrary::execGetRandomTowerByRarity)
{
    P_GET_OBJECT(UObject,LegacyContext);
    P_GET_PROPERTY(FByteProperty,CurrentType);
    Stack.StepCompiledIn<FStructProperty>(nullptr);
    auto* P=CastField<FStructProperty>(Stack.MostRecentProperty);
    void* Address=Stack.MostRecentPropertyAddress;
    P_FINISH;
    P_NATIVE_BEGIN;
    FTDDataRules::GetRandomTowerByRarity(LegacyContext,CurrentType,P,Address);
    P_NATIVE_END;
}
