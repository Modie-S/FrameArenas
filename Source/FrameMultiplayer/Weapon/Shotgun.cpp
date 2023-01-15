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
        TMap<AFrameCharacter*, uint32> HeadShotHitMap;
        for (FVector_NetQuantize HitTarget : HitTargets)
        {
            FHitResult FireHit;
            WeaponTraceHit(Start, HitTarget, FireHit);

            AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(FireHit.GetActor());
                if (FrameCharacter)
                {
                    const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");

                    if (bHeadShot)
                    {
                        if (HeadShotHitMap.Contains(FrameCharacter)) HeadShotHitMap[FrameCharacter]++;
                        else HeadShotHitMap.Emplace(FrameCharacter, 1);
                    }
                    else
                    {
                        if (HitMap.Contains(FrameCharacter)) HitMap[FrameCharacter]++;
                        else HitMap.Emplace(FrameCharacter, 1);
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
        
        TMap<AFrameCharacter*, float> DamageMap; // Maps character hit to the total damage inflicted

        for (auto HitPair : HitMap) // Body shot damage calculated by * times hit by damage - stored in DamageMap
        {
            if (HitPair.Key)
            {
                DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

                HitCharacters.AddUnique(HitPair.Key);               
            }
        }

        for (auto HeadShotHitPair : HeadShotHitMap) // Head shot damage calculated by * times hit by damage - stored in DamageMap
        {
            if (HeadShotHitPair.Key)
            {
                if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
                else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);

                HitCharacters.AddUnique(HeadShotHitPair.Key);               
            }
        }

        for (auto DamagePair : DamageMap) // Loop through DamageMap
        {
            if (DamagePair.Key && InstigatorController)
            {
                bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
                if (HasAuthority() && bCauseAuthDamage)
                {
                    UGameplayStatics::ApplyDamage(
                        DamagePair.Key,    // Character that was hit
                        DamagePair.Value, // Damage calculated in for loops above
                        InstigatorController,
                        this,
                        UDamageType::StaticClass()
                    );
                } 
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
