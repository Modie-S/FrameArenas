// MaxiMod Games 2022
// Modie Shakarchi


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget);
  
    APawn* InstigatorPawn = Cast<APawn>(GetOwner());
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash_Socket_01"));
    UWorld* World = GetWorld();
    if (MuzzleFlashSocket && World)
    {
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        // From muzzle socket to hit location from TraceUnderCrosshairs()
        FVector ToTarget = HitTarget - SocketTransform.GetLocation();
        FRotator TargetRotation = ToTarget.Rotation();
        
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = InstigatorPawn;

        AProjectile* SpawnedProjectile = nullptr;
        
        if (bUseServerSideRewind)
        {
            if (InstigatorPawn->HasAuthority()) // On server - not using SSR
            {
                if (InstigatorPawn->IsLocallyControlled()) // Host server; use replicated projectile
                {
                    SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnedProjectile->bUseServerSideRewind = false;
                    SpawnedProjectile->Damage = Damage; // Damage value from Weapon.h
                }
                else // Server not locally controlled; non-replicated projectile spawned (SSR)
                {
                    SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnedProjectile->bUseServerSideRewind = true;
                }
            }
            else // Client - using SSR
            {
                if (InstigatorPawn->IsLocallyControlled()) // Client locally controlled; spawn non-replicated projectile (IS USING SSR)
                {
                    SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnedProjectile->bUseServerSideRewind = true;
                    SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
                    SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
                    SpawnedProjectile->Damage = Damage;

                }
                else // Client not locally controlled; non-replicated projectile spawned (NO SSR)
                {
                    SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnedProjectile->bUseServerSideRewind = false;
                }
            }
        }
        else // Weapon not using SSR
        {
            if (InstigatorPawn->HasAuthority())
            {
                SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                SpawnedProjectile->bUseServerSideRewind = false;
                SpawnedProjectile->Damage = Damage;
            }
        }
    }
}
