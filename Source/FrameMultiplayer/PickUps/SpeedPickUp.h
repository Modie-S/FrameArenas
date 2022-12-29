// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUps.h"
#include "SpeedPickUp.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API ASpeedPickUp : public APickUps
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
	float BaseSpeedBuff = 1600.f;
	
	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff = 850.f;

	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 30.f;
	
};
