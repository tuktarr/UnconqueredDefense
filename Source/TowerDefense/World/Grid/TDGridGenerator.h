#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDGridGenerator.generated.h"

class ATDTowerSlot;

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDGridGenerator : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Grid") void GenerateGrid();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid", meta=(ClampMin="1")) int32 GridSizeX = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid", meta=(ClampMin="1")) int32 GridSizeY = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid", meta=(ClampMin="1.0")) double TileSpacing = 100.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid") TSubclassOf<ATDTowerSlot> TowerSlotClass;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid") TArray<TObjectPtr<ATDTowerSlot>> SpawnedSlots;
};
