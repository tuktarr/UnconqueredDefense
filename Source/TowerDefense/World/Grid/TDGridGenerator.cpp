#include "World/Grid/TDGridGenerator.h"

#include "World/Grid/TDTowerSlot.h"

void ATDGridGenerator::GenerateGrid()
{
    if (!TowerSlotClass)
    {
        return;
    }

    for (ATDTowerSlot* Slot : SpawnedSlots)
    {
        if (IsValid(Slot))
        {
            Slot->Destroy();
        }
    }
    SpawnedSlots.Reset();

    const FVector Origin = GetActorLocation();
    for (int32 X = 0; X < GridSizeX; ++X)
    {
        for (int32 Y = 0; Y < GridSizeY; ++Y)
        {
            const FVector Location = Origin + FVector(X * TileSpacing, Y * TileSpacing, 0.0);
            if (ATDTowerSlot* Slot = GetWorld()->SpawnActor<ATDTowerSlot>(TowerSlotClass, Location, GetActorRotation()))
            {
                SpawnedSlots.Add(Slot);
            }
        }
    }
}
