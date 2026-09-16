#pragma once
#include "Commandlets/Commandlet.h"
#include "TDMigrateCommandlet.generated.h"

UCLASS()
class UTDMigrateCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UTDMigrateCommandlet();
    virtual int32 Main(const FString& Params) override;
};
