// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameAnimInstance.h"
#include "FrameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


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
    bIsCrouched = FrameCharacter->bIsCrouched;
    bAiming = FrameCharacter->IsAiming();

    FRotator AimRotation = FrameCharacter->GetBaseAimRotation();
    FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(FrameCharacter->GetVelocity());
    FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
    DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 15.f);
    YawOffset = DeltaRotation.Yaw;

    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = FrameCharacter->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const float Target = Delta.Yaw / DeltaTime;
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);
    
}    
