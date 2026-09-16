#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TDGameplayLibrary.generated.h"

class AAIController;

/** Compatibility facade forwarding existing Blueprint calls to feature rule classes.
 * Blueprint variables remain the
 * serialization boundary so maps, data tables, widgets and animation assets keep
 * their original references and defaults during migration. */
UCLASS()
class TOWERDEFENSE_API UTDGameplayLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void TowerBossSpawned(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void TowerSold(UObject* Context, double Amount);
    // Cursor-independent entry used by the input handler and deterministic tests.
    static void ControllerInteract(UObject* Context, AActor* Actor, bool HitSomething);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void ControllerTick(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void UpdateGhostTowerDisplay(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void ToggleBuildMode(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void BeginMergeMode(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void BeginSellMode(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void EndTowerMode(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void ControllerClick(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void UpgradeTowerRank(UObject* Context, uint8 PreviousTower, uint8& UpgradeTower);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MonsterActivate(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MonsterDeactivate(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MonsterSpawnFromPool(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MonsterDamage(UObject* Context, float Damage);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MonsterReward(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MonsterRoundChanged(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void PortalBeginPlay(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void PortalCreateMonster(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void PortalSpawnMonster(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void PortalResumeSpawning(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void PortalResetBoss(UObject* Context);
    UFUNCTION(BlueprintCallable, CustomThunk, Category="TD|Rules", meta=(DefaultToSelf="Context", CustomStructureParam="CurrentRoundData")) static void GetCurrentRoundData(UObject* Context, int32& CurrentRoundData);
    DECLARE_FUNCTION(execGetCurrentRoundData);
    UFUNCTION(BlueprintCallable, CustomThunk, Category="TD|Rules", meta=(DefaultToSelf="Context", CustomStructureParam="InitStat")) static void InitializeTower(UObject* Context, FName RowName, int32& InitStat);
    DECLARE_FUNCTION(execInitializeTower);
    UFUNCTION(BlueprintCallable, CustomThunk, Category="TD|Rules", meta=(DefaultToSelf="Context", CustomStructureParam="SelectedTower")) static void GetRandomTowerByRarity(UObject* Context, uint8 CurrentType, int32& SelectedTower);
    DECLARE_FUNCTION(execGetRandomTowerByRarity);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void TowerBeginOverlap(UObject* Context, AActor* OtherActor);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void TowerEndOverlap(UObject* Context, AActor* OtherActor);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void TowerAttackNotify(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void TowerAttackEndNotify(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void DestroyAndFreeSlot(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void SpendGold(UObject* Context, double Amount, bool& Success);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void AddGold(UObject* Context, double Amount);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void AddMonsterCount(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MinusMonsterCount(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void GoNextRound(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void UpdateRoundData(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void GameModeTick(UObject* Context, float DeltaSeconds);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MoveToMouseDirection(UObject* Context, double PosX, double PosY, double& PosX_2, double& PosY_2);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void PlayerTick(UObject* Context, float DeltaSeconds);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void PlayerZoom(UObject* Context, float ActionValue);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void InitializeHealth(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void UpdateMaxHealth(UObject* Context, double MaxHP);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void ReceiveDamage(UObject* Context, double Damage);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void SwordAction(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void GunAction(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void RobotAction(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MagicAction(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void SupportAction(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void ProjectileOverlap(UObject* Context, AActor* OtherActor);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void CannonOverlap(UObject* Context, AActor* OtherActor);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void ActivateProjectile(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void DeactivateProjectile(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void GenerateGrid(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void ClearGhostTower(UObject* Context);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void MonsterPossess(UObject* Context, APawn* PossessedPawn);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void ClearAITarget(UObject* Context, AAIController* OwnerController, APawn* ControlledPawn);
    UFUNCTION(BlueprintCallable, Category="TD|Rules", meta=(DefaultToSelf="Context")) static void FindNextCheckpoint(UObject* Context, AAIController* OwnerController, APawn* ControlledPawn, float DeltaSeconds);
};
