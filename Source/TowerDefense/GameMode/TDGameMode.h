#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Data/TDTypes.h"
#include "TDGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDNumberChanged, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDIntegerChanged, int32, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDSimpleEvent);

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATDGameMode();
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="Economy")
    bool SpendGold(float Amount);

    UFUNCTION(BlueprintCallable, Category="Economy")
    void AddGold(float Amount);

    UFUNCTION(BlueprintCallable, Category="Round")
    void AddMonsterCount();

    UFUNCTION(BlueprintCallable, Category="Round")
    void MinusMonsterCount();

    UFUNCTION(BlueprintCallable, Category="Round")
    void GoNextRound();

    UFUNCTION(BlueprintCallable, Category="Tower")
    bool GetRandomTowerByRarity(ETDRarity Rarity, FTD_TowerStatData& OutTower) const;

    UFUNCTION(BlueprintCallable, Category="Round")
    bool UpdateRoundData();

    UFUNCTION(BlueprintCallable, Category="Round")
    void NotifyBossSpawned() { OnBossSpawned.Broadcast(); }

    UFUNCTION(BlueprintCallable, Category="Round")
    void NotifyBossDied() { OnBossDied.Broadcast(); }

    UPROPERTY(BlueprintAssignable, Category="Economy") FTDNumberChanged OnGoldChanged;
    UPROPERTY(BlueprintAssignable, Category="Round") FTDIntegerChanged OnMonsterNumChanged;
    UPROPERTY(BlueprintAssignable, Category="Round") FTDIntegerChanged OnRoundChanged;
    UPROPERTY(BlueprintAssignable, Category="Round") FTDNumberChanged OnTimerUpdated;
    UPROPERTY(BlueprintAssignable, Category="Round") FTDSimpleEvent OnBossSpawned;
    UPROPERTY(BlueprintAssignable, Category="Round") FTDSimpleEvent OnBossDied;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Economy") float Gold = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Economy") float NormalTowerCost = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Round") int32 MonsterCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Round") int32 CurrentRound = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Round") int32 MaxRound = 10;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round") float RemainingTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data") TObjectPtr<UDataTable> TowerData;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data") TObjectPtr<UDataTable> RoundData;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Round") FTD_RoundData CurrentRoundData;

    UFUNCTION(BlueprintImplementableEvent, Category="Round") void GameOver();
    UFUNCTION(BlueprintImplementableEvent, Category="Round") void GameClear();
};
