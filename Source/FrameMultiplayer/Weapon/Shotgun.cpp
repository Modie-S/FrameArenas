// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AShotgun::Fire(const FVector& HitTarget)
{
    AWeapon::Fire(HitTarget);
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr) return;
    AController* InstigatorController = OwnerPawn->GetController();
    
  

    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash_Socket_01");
    if (MuzzleFlashSocket)
    {
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation() + 100.f;
        for (uint32 i = 0; i < NumberOfBullets; i++)
        {
            FVector End = TraceEndScatter(Start, HitTarget);
        }
        

        
    }
}


