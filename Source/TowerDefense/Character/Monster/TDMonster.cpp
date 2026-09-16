#include "Character/Monster/TDMonster.h"

#include "GameMode/TDGameMode.h"
#include "Components/Health/TDHealthComponent.h"

ATDMonster::ATDMonster()
{
    HealthComponent = CreateDefaultSubobject<UTDHealthComponent>(TEXT("HealthComponent"));
    HealthComponent->OnDead.AddDynamic(this, &ATDMonster::HandleDeath);
}

void ATDMonster::ActivateFromPool(const FVector& Location)
{
    SetActorLocation(Location);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    bIsActive = true;
    OnMonsterActivated.Broadcast(this);

    if (ATDGameMode* GameMode = GetWorld()->GetAuthGameMode<ATDGameMode>())
    {
        GameMode->AddMonsterCount();
    }
}

void ATDMonster::DeactivateToPool()
{
    const bool bWasActive = bIsActive;
    bIsActive = false;
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
    if (bWasActive)
    {
        OnMonsterDeactivated.Broadcast(this);
    }
}

void ATDMonster::InitializeMonster(const double MaxHealth, const double InArmor, const int32 InRewardGold, const bool bInIsBoss)
{
    Armor = FMath::Max(0.0, InArmor);
    RewardGold = FMath::Max(0, InRewardGold);
    bIsBoss = bInIsBoss;
    HealthComponent->InitializeHealth(MaxHealth);
}

float ATDMonster::TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (bIsActive)
    {
        HealthComponent->ReceiveDamage(FMath::Max(0.0, static_cast<double>(AppliedDamage) - Armor));
    }
    return AppliedDamage;
}

void ATDMonster::HandleDeath()
{
    if (ATDGameMode* GameMode = GetWorld()->GetAuthGameMode<ATDGameMode>())
    {
        GameMode->AddGold(RewardGold);
        GameMode->MinusMonsterCount();
        if (bIsBoss)
        {
            GameMode->NotifyBossDied();
        }
    }
    DeactivateToPool();
}
