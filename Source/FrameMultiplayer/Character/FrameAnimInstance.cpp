// MaxiMod Games 2022
// Modie Shakarchi


#include "FrameAnimInstance.h"
#include "FrameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "FrameMultiplayer/Weapon/Weapon.h"
#include "FrameMultiplayer/Types/CombatState.h"


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
    EquippedWeapon = FrameCharacter->GetEquippedWeapon();
    bIsCrouched = FrameCharacter->bIsCrouched;
    bAiming = FrameCharacter->IsAiming();
    bElimmed = FrameCharacter->IsElimmed();
   

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

    if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && FrameCharacter->GetMesh())
    {
        LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation;
        FrameCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));

        if (FrameCharacter->IsLocallyControlled())
        {
            bLocallyControlled = true;
            FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - FrameCharacter->GetHitTarget()));
            RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
        }
    }

    bUseFABRIK = FrameCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
    if (FrameCharacter->IsLocallyControlled() && FrameCharacter->GetCombatState() != ECombatState::ECS_Throwing)
    {
        bUseFABRIK = !FrameCharacter->IsLocallyReloading();
    }
    bUseAimOffsets = FrameCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !FrameCharacter->GetDisableGameplay();
    bTransformRightHand = FrameCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !FrameCharacter->GetDisableGameplay();
    
}    
