// MaxiMod Games 2023
// Modie Shakarchi


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "WeaponTypes.h"


void AHitScanWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget);

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr) return;
    AController* InstigatorController = OwnerPawn->GetController();
    
  
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash_Socket_01");
    if (MuzzleFlashSocket)
    {
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation() + 15.f;
        
        FHitResult FireHit;
        WeaponTraceHit(Start, HitTarget, FireHit);

        AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(FireHit.GetActor());
        if (FrameCharacter && HasAuthority() && InstigatorController)
        {
            UGameplayStatics::ApplyDamage(
                FrameCharacter,
                Damage,
                InstigatorController,
                this,
                UDamageType::StaticClass()
            );
        }
        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(),
                ImpactParticles,
                FireHit.ImpactPoint,
                FireHit.ImpactNormal.Rotation()
            );
        }
        if (HitSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                HitSound,
                FireHit.ImpactPoint
            );
        }        
        if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
    }
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
    UWorld* World = GetWorld();
    if (World)
    {
            FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
            
            World->LineTraceSingleByChannel(
                OutHit,
                TraceStart,
                End,
                ECollisionChannel::ECC_Visibility
            );
            
            FVector BeamEnd = End;
            if (OutHit.bBlockingHit)
            {
                BeamEnd = OutHit.ImpactPoint;
            }

             DrawDebugSphere(
                GetWorld(),
                BeamEnd,
                16.f,
                12,
                FColor::Red,
                true
                );
           
            if (BeamParticles)
            {
                UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
                    World,
                    BeamParticles,
                    TraceStart,
                    FRotator::ZeroRotator,
                    true
                );
                if (Beam)
                {
                    Beam->SetVectorParameter(FName("Target"), BeamEnd);
                }
            }

    }
}



