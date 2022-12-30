// MaxiMod Games 2022
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "PickUps.h"
#include "ShieldPickUp.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AShieldPickUp : public APickUps
{
	GENERATED_BODY()

protected:

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult
		) override;

private:

	UPROPERTY(EditAnywhere)
	float ShieldRecharge = 100.f;

	UPROPERTY(EditAnywhere)
	float ShieldRechargeTime = 5.f;
	
};
