#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDTowerSlot.generated.h"

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDTowerSlot : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Tower Slot") bool TryOccupy();
    UFUNCTION(BlueprintCallable, Category="Tower Slot") void Release();
    UFUNCTION(BlueprintPure, Category="Tower Slot") bool IsOccupied() const { return bIsOccupied; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tower Slot") bool bIsOccupied = false;
};
