// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameAnimInstance.h"
#include "FrameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UFrameAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    FrameCharacter = Cast<AFrameCharacter>(TryGetPawnOwner());
}

void UFrameAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (FrameCharacter == nullptr)
    {
        FrameCharacter = Cast<AFrameCharacter>(TryGetPawnOwner());
    }

    if (FrameCharacter == nullptr) return;

    FVector Velocity = FrameCharacter->GetVelocity();
    Velocity.Z = 0.f;
    Speed = Velocity.Size();

    bIsInAir = FrameCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = FrameCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
    bWeaponEquipped = FrameCharacter->IsWeaponEquipped();
}    
