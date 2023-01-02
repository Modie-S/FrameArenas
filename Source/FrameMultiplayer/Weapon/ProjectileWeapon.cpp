// MaxiMod Games 2022
// Modie Shakarchi


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget);

    if (!HasAuthority()) return;
    
    APawn* InstigatorPawn = Cast<APawn>(GetOwner());
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash_Socket_01"));
    if (MuzzleFlashSocket)
    {
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        // From muzzle socket to hit location from TraceUnderCrosshairs()
        FVector ToTarget = HitTarget - SocketTransform.GetLocation();
        FRotator TargetRotation = ToTarget.Rotation();
        if (ProjectileClass && InstigatorPawn)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = GetOwner();
            SpawnParams.Instigator = InstigatorPawn;
            UWorld* World = GetWorld();
            if (World)
            {
                World->SpawnActor<AProjectile>(
                    ProjectileClass,
                    SocketTransform.GetLocation(),
                    TargetRotation,
                    SpawnParams
                    );
            }
        }
    }
}
