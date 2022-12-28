// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUps.h"
#include "FrameMultiplayer/Weapon/WeaponTypes.h"
#include "AmmoPickUp.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AAmmoPickUp : public APickUps
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
	int32 AmmoAmount = 30.f;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

};
