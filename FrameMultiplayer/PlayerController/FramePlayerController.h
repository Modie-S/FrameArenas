// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FramePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AFramePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDElims(int32 Elims);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	virtual void OnPossess(APawn* InPawn) override;
	
protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
	class AFrameHUD* FrameHUD;
};
