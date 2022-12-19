// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
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
            FVector End = bUseScatter ? TraceEndScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
            
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


FVector AHitScanWeapon::TraceEndScatter(const FVector& TraceStart, const FVector& HitTarget)
{
    FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
    FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
    FVector EndLoc = SphereCenter + RandomVector;
    FVector ToEndLoc = EndLoc - TraceStart;

    /*DrawDebugSphere(
        GetWorld(),
        SphereCenter,
        SphereRadius,
        12,
        FColor::Red,
        true
    );

    DrawDebugSphere(
        GetWorld(),
        EndLoc,
        4.f,
        12,
        FColor::Orange,
        true
    );

    DrawDebugLine(
        GetWorld(),
        TraceStart,
        FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
        FColor::Cyan,
        true
    );*/

    return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}


