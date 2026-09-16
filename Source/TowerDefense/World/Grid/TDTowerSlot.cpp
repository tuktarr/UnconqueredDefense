#include "World/Grid/TDTowerSlot.h"

bool ATDTowerSlot::TryOccupy()
{
    if (bIsOccupied)
    {
        return false;
    }
    bIsOccupied = true;
    return true;
}

void ATDTowerSlot::Release()
{
    bIsOccupied = false;
}
