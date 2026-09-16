#include "Character/Tower/TDSpecializedTowers.h"

#include "Kismet/GameplayStatics.h"
#include "GameMode/TDGameMode.h"
#include "Character/Monster/TDMonster.h"
#include "Character/Tower/Projectile/TDProjectile.h"

void ATDMeleeTower::TowerAction()
{
    Super::TowerAction();
    RemoveInvalidTargets();
    if (Targets.IsEmpty())
    {
        return;
    }
    UGameplayStatics::ApplyDamage(Targets[0], Stat.Damage, GetInstigatorController(), this, UDamageType::StaticClass());
}

void ATDProjectileTower::TowerAction()
{
    Super::TowerAction();
    RemoveInvalidTargets();
    if (Targets.IsEmpty() || !ProjectileClass)
    {
        return;
    }

    const FVector TargetLocation = Targets[0]->GetActorLocation();
    const FRotator Rotation = (TargetLocation - GetActorLocation()).Rotation();
    const FTransform SpawnTransform(Rotation, GetActorLocation());
    if (ATDProjectile* Projectile = GetWorld()->SpawnActorDeferred<ATDProjectile>(ProjectileClass, SpawnTransform, this, GetInstigator()))
    {
        UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
        Projectile->ActivateProjectile(SpawnTransform, Stat.Damage, this);
    }
}

void ATDAreaTower::TowerAction()
{
    Super::TowerAction();
    RemoveInvalidTargets();
    if (Targets.IsEmpty())
    {
        return;
    }
    UGameplayStatics::ApplyRadialDamage(this, Stat.Damage, Targets[0]->GetActorLocation(), EffectRadius,
        UDamageType::StaticClass(), {}, this, GetInstigatorController());
}

void ATDSupportTower::TowerAction()
{
    Super::TowerAction();
    if (ATDGameMode* GameMode = GetWorld()->GetAuthGameMode<ATDGameMode>())
    {
        GameMode->AddGold(GoldPerAction);
    }
}
