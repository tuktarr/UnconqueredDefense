#include "Character/Tower/Projectile/TDProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Monster/TDMonster.h"

ATDProjectile::ATDProjectile()
{
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ATDProjectile::HandleOverlap);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bAutoActivate = false;
}

void ATDProjectile::ActivateProjectile(const FTransform& Transform, const double Damage, AActor* NewOwner)
{
    SetOwner(NewOwner);
    SetActorTransform(Transform);
    ProjectileDamage = FMath::Max(0.0, Damage);
    bIsActive = true;
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    ProjectileMovement->Activate(true);
}

void ATDProjectile::DeactivateProjectile()
{
    bIsActive = false;
    ProjectileMovement->StopMovementImmediately();
    ProjectileMovement->Deactivate();
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

void ATDProjectile::HandleOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (!bIsActive || !OtherActor || OtherActor == GetOwner() || !OtherActor->IsA<ATDMonster>())
    {
        return;
    }
    UGameplayStatics::ApplyDamage(OtherActor, ProjectileDamage, GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
    DeactivateProjectile();
}
