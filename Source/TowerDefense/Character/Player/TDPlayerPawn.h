#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TDPlayerPawn.generated.h"

class USpringArmComponent;

UCLASS(Blueprintable)
class TOWERDEFENSE_API ATDPlayerPawn : public APawn
{
    GENERATED_BODY()

public:
    ATDPlayerPawn();
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="Player")
    FVector2D MoveToMouseDirection(const FVector2D& MousePosition) const;

    UFUNCTION(BlueprintCallable, Category="Player")
    void ApplyZoom(float AxisValue);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player")
    float ScrollSpeed = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player")
    float EdgeThreshold = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Zoom")
    float MinimumArmLength = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Zoom")
    float MaximumArmLength = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Zoom")
    TObjectPtr<USpringArmComponent> SpringArm;
};
