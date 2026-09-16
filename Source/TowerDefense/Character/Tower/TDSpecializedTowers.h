#pragma once

#include "CoreMinimal.h"
#include "Character/Tower/TDTowerBase.h"
#include "TDSpecializedTowers.generated.h"

class ATDProjectile;

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDMeleeTower : public ATDTowerBase
{
    GENERATED_BODY()
public:
    virtual void TowerAction() override;
};

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDProjectileTower : public ATDTowerBase
{
    GENERATED_BODY()
public:
    virtual void TowerAction() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Projectile") TSubclassOf<ATDProjectile> ProjectileClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Projectile") FName MuzzleSocketName = TEXT("Muzzle");
};

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDAreaTower : public ATDTowerBase
{
    GENERATED_BODY()
public:
    virtual void TowerAction() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Area") double EffectRadius = 200.0;
};

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDSupportTower : public ATDTowerBase
{
    GENERATED_BODY()
public:
    virtual void TowerAction() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Support") double GoldPerAction = 1.0;
};

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDRobotTower : public ATDProjectileTower
{
    GENERATED_BODY()
};
