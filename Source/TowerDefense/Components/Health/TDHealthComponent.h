#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDHealthDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTDHealthChanged, double, Health, double, MaxHealth);

UCLASS(ClassGroup=(TowerDefense), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class TOWERDEFENSE_API UTDHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Health") void InitializeHealth(double InMaxHealth);
    UFUNCTION(BlueprintCallable, Category="Health") double ReceiveDamage(double Damage);

    UPROPERTY(BlueprintAssignable, Category="Health") FTDHealthDead OnDead;
    UPROPERTY(BlueprintAssignable, Category="Health") FTDHealthChanged OnHealthChanged;

    UFUNCTION(BlueprintPure, Category="Health") double GetHealth() const { return Health; }
    UFUNCTION(BlueprintPure, Category="Health") double GetMaxHealth() const { return MaxHealth; }
    UFUNCTION(BlueprintPure, Category="Health") bool IsDead() const { return Health <= 0.0; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health") double MaxHealth = 100.0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health") double Health = 100.0;
};
