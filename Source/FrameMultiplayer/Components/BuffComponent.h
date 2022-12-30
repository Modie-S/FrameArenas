// MaxiMod Games 2022
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRAMEMULTIPLAYER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UBuffComponent();
	friend class AFrameCharacter;
	void Heal(float HealAmount, float HealingTime);
	void RechargeShield(float ShieldAmount, float RechargeTime);
	void SpeedBuff(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float Velocity);
	void JumpBuff(float BuffJumpVelocity, float BuffTime);

protected:
	
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);

private:

	UPROPERTY()
	class AFrameCharacter* Character;

	//
	// Health
	//
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	//
	// Shield
	//
	bool bRecharging = false;
	float RechargeRate = 0.f;
	float AmountToRecharge = 0.f;

	//
	// Speed
	//
	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);
	
	//
	// Jump
	//
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

	

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
