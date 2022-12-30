// MaxiMod Games 2022
// Modie Shakarchi


#include "ShieldPickUp.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/Components/BuffComponent.h"

void AShieldPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(OtherActor);
    if (FrameCharacter)
    {
       UBuffComponent* Buff = FrameCharacter->GetBuff();
       if (Buff)
       {
            Buff->RechargeShield(ShieldRecharge, ShieldRechargeTime);
       }
    }

    Destroy();
}

