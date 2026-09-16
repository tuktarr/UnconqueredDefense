#include "Components/Health/TDHealthComponent.h"

void UTDHealthComponent::InitializeHealth(const double InMaxHealth)
{
    MaxHealth = FMath::Max(0.0, InMaxHealth);
    Health = MaxHealth;
    OnHealthChanged.Broadcast(Health, MaxHealth);
}

double UTDHealthComponent::ReceiveDamage(const double Damage)
{
    if (Damage <= 0.0 || IsDead())
    {
        return Health;
    }

    Health = FMath::Max(0.0, Health - Damage);
    OnHealthChanged.Broadcast(Health, MaxHealth);
    if (IsDead())
    {
        OnDead.Broadcast();
    }
    return Health;
}
