#include "Character/Player/TDPlayerPawn.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

ATDPlayerPawn::ATDPlayerPawn()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATDPlayerPawn::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController)
    {
        return;
    }

    float MouseX = 0.0f;
    float MouseY = 0.0f;
    if (!PlayerController->GetMousePosition(MouseX, MouseY))
    {
        return;
    }

    const FVector2D Direction = MoveToMouseDirection(FVector2D(MouseX, MouseY));
    AddMovementInput(GetActorForwardVector(), Direction.Y);
    AddMovementInput(GetActorRightVector(), Direction.X);
}

FVector2D ATDPlayerPawn::MoveToMouseDirection(const FVector2D& MousePosition) const
{
    const APlayerController* PlayerController = Cast<APlayerController>(GetController());
    int32 Width = 0;
    int32 Height = 0;
    if (!PlayerController)
    {
        return FVector2D::ZeroVector;
    }
    PlayerController->GetViewportSize(Width, Height);

    const float X = MousePosition.X < EdgeThreshold ? -1.0f
        : MousePosition.X > Width - EdgeThreshold ? 1.0f : 0.0f;
    const float Y = MousePosition.Y < EdgeThreshold ? 1.0f
        : MousePosition.Y > Height - EdgeThreshold ? -1.0f : 0.0f;
    return FVector2D(X, Y) * ScrollSpeed;
}

void ATDPlayerPawn::ApplyZoom(const float AxisValue)
{
    if (!SpringArm)
    {
        return;
    }
    SpringArm->TargetArmLength = FMath::Clamp(
        SpringArm->TargetArmLength + AxisValue * ScrollSpeed,
        MinimumArmLength,
        MaximumArmLength);
}
