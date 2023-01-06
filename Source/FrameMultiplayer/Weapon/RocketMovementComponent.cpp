// MaxiMod Games 2023
// Modie Shakarchi


#include "RocketMovementComponent.h"


URocketMovementComponent::EHandleBlockingHitResult URocketMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
    Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
    return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void URocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
    // Rocket should not stop - only explode when collision hit detected
}
