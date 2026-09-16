#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDMonster.generated.h"

class UTDHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDMonsterEvent, class ATDMonster*, Monster);

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDMonster : public ACharacter
{
    GENERATED_BODY()

public:
    ATDMonster();

    UFUNCTION(BlueprintCallable, Category="Pool") void ActivateFromPool(const FVector& Location);
    UFUNCTION(BlueprintCallable, Category="Pool") void DeactivateToPool();
    UFUNCTION(BlueprintCallable, Category="Monster") void InitializeMonster(double MaxHealth, double InArmor, int32 InRewardGold, bool bInIsBoss);

    UPROPERTY(BlueprintAssignable, Category="Monster") FTDMonsterEvent OnMonsterActivated;
    UPROPERTY(BlueprintAssignable, Category="Monster") FTDMonsterEvent OnMonsterDeactivated;

    UFUNCTION(BlueprintPure, Category="Monster") bool IsActive() const { return bIsActive; }
    UFUNCTION(BlueprintPure, Category="Monster") UTDHealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION() void HandleDeath();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Monster") TObjectPtr<UTDHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Monster") bool bIsActive = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster") double Armor = 0.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster") int32 RewardGold = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster") bool bIsBoss = false;
};
