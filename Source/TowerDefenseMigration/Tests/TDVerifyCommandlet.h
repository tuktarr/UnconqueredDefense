#pragma once
#include "Commandlets/Commandlet.h"
#include "TDVerifyCommandlet.generated.h"

/** Repeatable headless state checks. No content is saved. */
UCLASS()
class UTDVerifyCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UTDVerifyCommandlet();
    virtual int32 Main(const FString& Params) override;
    UFUNCTION() void OnDeath() { ++Deaths; }
    UFUNCTION() void OnHealth(double HP, double MaxHP) { ++HealthChanges; LastHP=HP; LastMaxHP=MaxHP; }
    int32 Deaths=0, HealthChanges=0;
    double LastHP=0, LastMaxHP=0;
};
