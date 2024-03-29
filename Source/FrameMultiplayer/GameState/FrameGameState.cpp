// MaxiMod Games 2023
// Modie Shakarchi


#include "FrameGameState.h"
#include "Net/UnrealNetwork.h"
#include "FrameMultiplayer/PlayerState/FramePlayerState.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"


void AFrameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrameGameState, TopScoringPlayers);
    DOREPLIFETIME(AFrameGameState, RedTeamScore);
    DOREPLIFETIME(AFrameGameState, BlueTeamScore);
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

void AFrameGameState::RedTeamScored()
{
    ++RedTeamScore;

    AFramePlayerController* FPlayer = Cast<AFramePlayerController>(GetWorld()->GetFirstPlayerController());
    if (FPlayer)
    {
        FPlayer->SetHUDRedTeamScore(RedTeamScore);
    }
}

void AFrameGameState::BlueTeamScored()
{
    ++BlueTeamScore;

    AFramePlayerController* FPlayer = Cast<AFramePlayerController>(GetWorld()->GetFirstPlayerController());
    if (FPlayer)
    {
        FPlayer->SetHUDBlueTeamScore(BlueTeamScore);
    }
}

void AFrameGameState::OnRep_RedTeamScore()
{
    AFramePlayerController* FPlayer = Cast<AFramePlayerController>(GetWorld()->GetFirstPlayerController());
    if (FPlayer)
    {
        FPlayer->SetHUDRedTeamScore(RedTeamScore);
    }
}

void AFrameGameState::OnRep_BlueTeamScore()
{
    AFramePlayerController* FPlayer = Cast<AFramePlayerController>(GetWorld()->GetFirstPlayerController());
    if (FPlayer)
    {
        FPlayer->SetHUDBlueTeamScore(BlueTeamScore);
    }
}
