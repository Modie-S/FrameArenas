// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickUp.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/Components/CombatComponent.h"


void AAmmoPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(OtherActor);
    if (FrameCharacter)
    {
        UCombatComponent* Combat = FrameCharacter->GetCombat();
        if (Combat)
        {
            Combat->PickUpAmmo(WeaponType, AmmoAmount);
        }
    }

    Destroy();
}
