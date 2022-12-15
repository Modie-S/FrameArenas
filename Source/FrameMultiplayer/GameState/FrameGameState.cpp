// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameGameState.h"
#include "Net/UnrealNetwork.h"
#include "FrameMultiplayer/PlayerState/FramePlayerState.h"


void AFrameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrameGameState, TopScoringPlayers);
}

void AFrameGameState::UpdateTopScore(class AFramePlayerState* ScoringPlayer)
{
    if (TopScoringPlayers.Num() == 0)
    {
        TopScoringPlayers.Add(ScoringPlayer);
        TopScore = ScoringPlayer->GetScore();
    }
    else if (ScoringPlayer->GetScore() == TopScore)
    {
        TopScoringPlayers.AddUnique(ScoringPlayer);
    }
    else if (ScoringPlayer->GetScore() > TopScore)
    {
        TopScoringPlayers.Empty();
        TopScoringPlayers.AddUnique(ScoringPlayer);
        TopScore = ScoringPlayer->GetScore();
    }
}
