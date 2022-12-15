// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FrameGameState.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AFrameGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AFramePlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<AFramePlayerState*> TopScoringPlayers;

private:

	float TopScore = 0.f;




};
