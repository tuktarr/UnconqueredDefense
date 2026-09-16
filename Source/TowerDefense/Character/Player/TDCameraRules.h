#pragma once

#include "CoreMinimal.h"

class AActor;
class APawn;
class AAIController;

// Stateless gameplay operations. State remains on the supplied content objects.
class TOWERDEFENSE_API FTDCameraRules final
{
public:
    static void MoveToMouseDirection(UObject* Context, double PosX, double PosY, double& PosX_2, double& PosY_2);
    static void PlayerTick(UObject* Context, float DeltaSeconds);
    static void PlayerZoom(UObject* Context, float ActionValue);
private:
    FTDCameraRules() = delete;
};
