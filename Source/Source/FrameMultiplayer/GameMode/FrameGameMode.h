// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FrameGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AFrameGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	virtual void PlayerEliminated(class AFrameCharacter* ElimmedCharacter, class AFramePlayerController* VictimController, AFramePlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
};
