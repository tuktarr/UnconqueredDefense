#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TDTypes.generated.h"

class UAnimInstance;
class USkeletalMesh;

UENUM(BlueprintType)
enum class ETDTowerType : uint8
{
    Gun,
    Magic,
    Sword,
    Support,
    Robot
};

UENUM(BlueprintType)
enum class ETDRarity : uint8
{
    Common,
    Normal,
    Rare,
    Epic,
    Special
};

UENUM(BlueprintType)
enum class ETDPlayerMode : uint8
{
    Build,
    Install,
    Merge,
    Sell
};

USTRUCT(BlueprintType)
struct FTD_TowerStatData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText TowerName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ETDRarity Rarity = ETDRarity::Common;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> TowerClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ETDTowerType TowerType = ETDTowerType::Gun;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double Damage = 0.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double AttackRange = 0.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double AttackRate = 1.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double SellPrice = 0.0;
};

USTRUCT(BlueprintType)
struct FTD_RoundData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RoundCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<USkeletalMesh> MonsterMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UAnimInstance> MonsterAnimClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsBoss = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double MaxHp = 100.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double MoveSpeed = 300.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RewardGold = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) double Armor = 0.0;
};
