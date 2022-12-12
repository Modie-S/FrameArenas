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

	AFrameGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AFrameCharacter* ElimmedCharacter, class AFramePlayerController* VictimController, AFramePlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmUpTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 600.f;

	float LevelStartingTime = 0.f;

protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:

	float CountdownTime = 0.f;
};
