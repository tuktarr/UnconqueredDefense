#pragma once
#include "Commandlets/Commandlet.h"
#include "TDInspectCommandlet.generated.h"

UCLASS()
class UTDInspectCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UTDInspectCommandlet();
    virtual int32 Main(const FString& Params) override;
};
