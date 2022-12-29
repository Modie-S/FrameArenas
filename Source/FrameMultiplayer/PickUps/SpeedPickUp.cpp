// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedPickUp.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/Components/BuffComponent.h"

void ASpeedPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(OtherActor);
    if (FrameCharacter)
    {
       UBuffComponent* Buff = FrameCharacter->GetBuff();
       if (Buff)
       {
            Buff->SpeedBuff(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
       }
    }

    Destroy();
}

