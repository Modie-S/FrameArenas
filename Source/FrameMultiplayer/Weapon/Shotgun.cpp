// MaxiMod Games 2023
// Modie Shakarchi


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "FrameMultiplayer/Components/LagCompensationComponent.h"


void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
    AWeapon::Fire(FVector());
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr) return;
    AController* InstigatorController = OwnerPawn->GetController();

    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash_Socket_01");
    if (MuzzleFlashSocket)
    {
        const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        const FVector Start = SocketTransform.GetLocation() + 15.f;
    

        TMap<AFrameCharacter*, uint32> HitMap; // Maps character hit to number of times hit
        for (FVector_NetQuantize HitTarget : HitTargets)
        {
            FHitResult FireHit;
            WeaponTraceHit(Start, HitTarget, FireHit);

            AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(FireHit.GetActor());
                if (FrameCharacter)
                {
                    if (HitMap.Contains(FrameCharacter))
                    {
                        HitMap[FrameCharacter]++;
                    }
                    else
                    {
                        HitMap.Emplace(FrameCharacter, 1);
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
                            FireHit.ImpactPoint,
                            0.5f,
                            FMath::FRandRange(-0.5, 0.5f)
                            );
                    }        
                }

        }
    
        TArray<AFrameCharacter*> HitCharacters;       
        for (auto HitPair : HitMap)
        {
            if (HitPair.Key && InstigatorController)
            {
                if (HasAuthority() && !bUseServerSideRewind)
                {
                    UGameplayStatics::ApplyDamage(
                        HitPair.Key,    // Character that was hit
                        Damage * HitPair.Value, // Multiply damage to number of times hit
                        InstigatorController,
                        this,
                        UDamageType::StaticClass()
                    );
                }

                HitCharacters.Add(HitPair.Key);               
            }
        }

        if (!HasAuthority() && bUseServerSideRewind)
        {
            FrameOwnerCharacter = FrameOwnerCharacter == nullptr ? Cast<AFrameCharacter>(OwnerPawn) : FrameOwnerCharacter;
            FrameOwnerController = FrameOwnerController == nullptr ? Cast<AFramePlayerController>(InstigatorController) : FrameOwnerController;
            if (FrameOwnerController && FrameOwnerCharacter && FrameOwnerCharacter->GetLagComp() && FrameOwnerCharacter->IsLocallyControlled())
            {
                FrameOwnerCharacter->GetLagComp()->ShotgunServerScoreRequest(
                        HitCharacters,
                        Start,
                        HitTargets,
                        FrameOwnerController->GetServerTime() - FrameOwnerController->SingleTripTime
                    );  
            }
        } 
    }
}

void AShotgun::ShotgunTraceEndScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash_Socket_01");
    if (MuzzleFlashSocket == nullptr) return;
    
    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
    const FVector TraceStart = SocketTransform.GetLocation() + 15.f;
    
    const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
    
    for (uint32 i = 0; i < NumberOfBullets; i++)
    {
        const FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
        const FVector EndLoc = SphereCenter + RandomVector;
        FVector ToEndLoc = EndLoc - TraceStart;
        ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size();
       
        HitTargets.Add(ToEndLoc);
    }
}
