#include "Character/Monster/Spawning/TDMonsterPool.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Character/Monster/TDMonster.h"

void ATDMonsterPool::BeginPlay()
{
    Super::BeginPlay();
    FillPool();
}

void ATDMonsterPool::FillPool()
{
    if (!MonsterClass)
    {
        return;
    }
    while (MonsterPool.Num() < PoolSize)
    {
        ATDMonster* Monster = GetWorld()->SpawnActor<ATDMonster>(MonsterClass, GetActorLocation(), GetActorRotation());
        if (!Monster)
        {
            break;
        }
        Monster->DeactivateToPool();
        Monster->SetActorHiddenInGame(true);
        Monster->SetActorEnableCollision(false);
        MonsterPool.Add(Monster);
    }
}

void ATDMonsterPool::StartWave(const int32 MonsterCount, const double SpawnInterval)
{
    RemainingToSpawn = FMath::Max(0, MonsterCount);
    if (RemainingToSpawn == 0)
    {
        return;
    }
    GetWorldTimerManager().SetTimer(SpawnTimer, this, &ATDMonsterPool::SpawnNextMonster,
        FMath::Max(0.01, SpawnInterval), true, 0.0f);
}

void ATDMonsterPool::PauseWave()
{
    GetWorldTimerManager().PauseTimer(SpawnTimer);
}

void ATDMonsterPool::ResumeWave()
{
    GetWorldTimerManager().UnPauseTimer(SpawnTimer);
}

void ATDMonsterPool::SpawnNextMonster()
{
    if (RemainingToSpawn <= 0)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimer);
        return;
    }
    ATDMonster* Monster = FindInactiveMonster();
    if (!Monster)
    {
        return;
    }

    const FVector Location = Waypoints.IsEmpty()
        ? GetActorLocation()
        : Waypoints[NextWaypointIndex++ % Waypoints.Num()]->GetActorLocation();
    Monster->ActivateFromPool(Location);
    --RemainingToSpawn;
}

ATDMonster* ATDMonsterPool::FindInactiveMonster() const
{
    for (ATDMonster* Monster : MonsterPool)
    {
        if (IsValid(Monster) && !Monster->IsActive())
        {
            return Monster;
        }
    }
    return nullptr;
}
