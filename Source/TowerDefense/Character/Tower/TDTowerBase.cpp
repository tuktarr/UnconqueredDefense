#include "Character/Tower/TDTowerBase.h"

#include "Components/SphereComponent.h"
#include "GameMode/TDGameMode.h"
#include "Character/Monster/TDMonster.h"

ATDTowerBase::ATDTowerBase()
{
    PrimaryActorTick.bCanEverTick = true;
    RangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));
    RootComponent = RangeSphere;
    RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ATDTowerBase::HandleTargetEntered);
    RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ATDTowerBase::HandleTargetLeft);
}

void ATDTowerBase::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    RemoveInvalidTargets();
    TimeUntilNextAction = FMath::Max(0.0, TimeUntilNextAction - DeltaSeconds);
    if (!Targets.IsEmpty() && TimeUntilNextAction <= 0.0)
    {
        TowerAction();
        TimeUntilNextAction = FMath::Max(0.01, Stat.AttackRate);
    }
}

void ATDTowerBase::InitializeTower(const FTD_TowerStatData& InStat)
{
    Stat = InStat;
    RangeSphere->SetSphereRadius(FMath::Max(0.0, Stat.AttackRange));
    TimeUntilNextAction = 0.0;
}

void ATDTowerBase::TowerAction()
{
    bIsActioning = !Targets.IsEmpty();
}

void ATDTowerBase::SellTower()
{
    if (ATDGameMode* GameMode = GetWorld()->GetAuthGameMode<ATDGameMode>())
    {
        GameMode->AddGold(Stat.SellPrice);
    }
    OnTowerSold.Broadcast(this);
    Destroy();
}

void ATDTowerBase::HandleTargetEntered(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (ATDMonster* Monster = Cast<ATDMonster>(OtherActor); Monster && Monster->IsActive())
    {
        Targets.AddUnique(Monster);
    }
}

void ATDTowerBase::HandleTargetLeft(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
    Targets.Remove(Cast<ATDMonster>(OtherActor));
}

void ATDTowerBase::RemoveInvalidTargets()
{
    Targets.RemoveAll([](const ATDMonster* Monster)
    {
        return !IsValid(Monster) || !Monster->IsActive();
    });
}
