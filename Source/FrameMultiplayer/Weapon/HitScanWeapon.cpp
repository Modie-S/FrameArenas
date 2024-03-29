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
#include "FrameMultiplayer/Components/LagCompensationComponent.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"


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
            bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
            if (HasAuthority() && bCauseAuthDamage)
            {
                const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
                                
                UGameplayStatics::ApplyDamage(
                    FrameCharacter,
                    DamageToCause,
                    InstigatorController,
                    this,
                    UDamageType::StaticClass()
                ); 
            }
            if (!HasAuthority() && bUseServerSideRewind)
            {
                FrameOwnerCharacter = FrameOwnerCharacter == nullptr ? Cast<AFrameCharacter>(OwnerPawn) : FrameOwnerCharacter;
                FrameOwnerController = FrameOwnerController == nullptr ? Cast<AFramePlayerController>(InstigatorController) : FrameOwnerController;
                if (FrameOwnerController && FrameOwnerCharacter && FrameOwnerCharacter->GetLagComp() && FrameOwnerCharacter->IsLocallyControlled())
                {
                    FrameOwnerCharacter->GetLagComp()->ServerScoreRequest(
                        FrameCharacter,
                        Start,
                        HitTarget,
                        FrameOwnerController->GetServerTime() - FrameOwnerController->SingleTripTime
                    );
                }
            }  
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
            else
            {
                OutHit.ImpactPoint = End;
            }

            /*DrawDebugSphere(
                GetWorld(),
                BeamEnd,
                16.f,
                12,
                FColor::Red,
                true
                );*/
           
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



