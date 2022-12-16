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
        FVector Start = SocketTransform.GetLocation() + 100.f;
        FVector End = Start + (HitTarget - Start) * 1.25f;

        FHitResult FireHit;
        UWorld* World = GetWorld();
        if (World)
        {
            World->LineTraceSingleByChannel(
                FireHit,
                Start,
                End,
                ECollisionChannel::ECC_Visibility
            );
            FVector BeamEnd = End;
            if (FireHit.bBlockingHit)
            {
                BeamEnd = FireHit.ImpactPoint;
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
                        World,
                        ImpactParticles,
                        FireHit.ImpactPoint,
                        FireHit.ImpactNormal.Rotation()
                    );
                }

            }
            if (BeamParticles)
            {
                UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
                    World,
                    BeamParticles,
                    SocketTransform
                );
                if (Beam)
                {
                    Beam->SetVectorParameter(FName("Target"), BeamEnd);
                }
            }
        }
        if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				World,
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

FVector AHitScanWeapon::TraceEndScatter(const FVector& TraceStart, const FVector& HitTarget)
{
    FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
    FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
    FVector EndLoc = SphereCenter + RandomVector;
    FVector ToEndLoc = EndLoc - TraceStart;

    DrawDebugSphere(
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
    );

    return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}
