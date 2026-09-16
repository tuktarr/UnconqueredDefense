#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDMonsterPool.generated.h"

class ATDMonster;

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDMonsterPool : public AActor
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Monster Pool") void FillPool();
    UFUNCTION(BlueprintCallable, Category="Monster Pool") void StartWave(int32 MonsterCount, double SpawnInterval);
    UFUNCTION(BlueprintCallable, Category="Monster Pool") void PauseWave();
    UFUNCTION(BlueprintCallable, Category="Monster Pool") void ResumeWave();

protected:
    UFUNCTION() void SpawnNextMonster();
    ATDMonster* FindInactiveMonster() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster Pool") TSubclassOf<ATDMonster> MonsterClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster Pool", meta=(ClampMin="1")) int32 PoolSize = 10;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster Pool") TArray<TObjectPtr<AActor>> Waypoints;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Monster Pool") TArray<TObjectPtr<ATDMonster>> MonsterPool;

    int32 RemainingToSpawn = 0;
    int32 NextWaypointIndex = 0;
    FTimerHandle SpawnTimer;
};
