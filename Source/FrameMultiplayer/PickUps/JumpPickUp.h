// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUps.h"
#include "JumpPickUp.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AJumpPickUp : public APickUps
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
	float JumpZVelocityBuff = 4000.f;

	UPROPERTY(EditAnywhere)
	float JumpBuffTime = 30.f;
};
