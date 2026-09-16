#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/TDTypes.h"
#include "TDTowerBase.generated.h"

class USphereComponent;
class ATDMonster;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDTowerSold, class ATDTowerBase*, Tower);

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDTowerBase : public AActor
{
    GENERATED_BODY()

public:
    ATDTowerBase();
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="Tower") void InitializeTower(const FTD_TowerStatData& InStat);
    UFUNCTION(BlueprintCallable, Category="Tower") virtual void TowerAction();
    UFUNCTION(BlueprintCallable, Category="Tower") void SellTower();

    UPROPERTY(BlueprintAssignable, Category="Tower") FTDTowerSold OnTowerSold;

protected:
    UFUNCTION() void HandleTargetEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION() void HandleTargetLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
    void RemoveInvalidTargets();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower") TObjectPtr<USphereComponent> RangeSphere;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tower") FTD_TowerStatData Stat;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower") TArray<TObjectPtr<ATDMonster>> Targets;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower") bool bIsActioning = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower") double TimeUntilNextAction = 0.0;
};
