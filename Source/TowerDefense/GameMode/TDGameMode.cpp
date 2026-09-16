#include "GameMode/TDGameMode.h"

#include "Engine/DataTable.h"

ATDGameMode::ATDGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATDGameMode::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (RemainingTime <= 0.0f)
    {
        return;
    }
    RemainingTime = FMath::Max(0.0f, RemainingTime - DeltaSeconds);
    OnTimerUpdated.Broadcast(RemainingTime);
    if (RemainingTime <= 0.0f && MonsterCount <= 0)
    {
        GoNextRound();
    }
}

bool ATDGameMode::SpendGold(const float Amount)
{
    if (Amount < 0.0f || Gold < Amount)
    {
        return false;
    }
    Gold -= Amount;
    OnGoldChanged.Broadcast(Gold);
    return true;
}

void ATDGameMode::AddGold(const float Amount)
{
    Gold += Amount;
    OnGoldChanged.Broadcast(Gold);
}

void ATDGameMode::AddMonsterCount()
{
    ++MonsterCount;
    OnMonsterNumChanged.Broadcast(MonsterCount);
}

void ATDGameMode::MinusMonsterCount()
{
    MonsterCount = FMath::Max(0, MonsterCount - 1);
    OnMonsterNumChanged.Broadcast(MonsterCount);
    if (MonsterCount == 0 && RemainingTime <= 0.0f)
    {
        GoNextRound();
    }
}

void ATDGameMode::GoNextRound()
{
    ++CurrentRound;
    OnRoundChanged.Broadcast(CurrentRound);
    if (CurrentRound > MaxRound)
    {
        GameClear();
        return;
    }
    UpdateRoundData();
}

bool ATDGameMode::GetRandomTowerByRarity(const ETDRarity Rarity, FTD_TowerStatData& OutTower) const
{
    if (!TowerData)
    {
        return false;
    }

    TArray<const FTD_TowerStatData*> Candidates;
    TowerData->ForeachRow<FTD_TowerStatData>(TEXT("GetRandomTowerByRarity"),
        [&Candidates, Rarity](const FName&, const FTD_TowerStatData& Row)
        {
            if (Row.Rarity == Rarity)
            {
                Candidates.Add(&Row);
            }
        });
    if (Candidates.IsEmpty())
    {
        return false;
    }
    OutTower = *Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
    return true;
}

bool ATDGameMode::UpdateRoundData()
{
    if (!RoundData)
    {
        return false;
    }
    const FName RowName(*FString::FromInt(CurrentRound));
    if (const FTD_RoundData* Row = RoundData->FindRow<FTD_RoundData>(RowName, TEXT("UpdateRoundData")))
    {
        CurrentRoundData = *Row;
        return true;
    }
    return false;
}
