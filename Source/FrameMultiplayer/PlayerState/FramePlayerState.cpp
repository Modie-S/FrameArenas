// MaxiMod Games 2023
// Modie Shakarchi


#include "FramePlayerState.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"
#include "Net/UnrealNetwork.h"


void AFramePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFramePlayerState, Elims);
    DOREPLIFETIME(AFramePlayerState, Team);
}



void AFramePlayerState::AddToScore(float ScoreAmount)
{
    SetScore(GetScore() + ScoreAmount);

    Character = Character == nullptr ? Cast<AFrameCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<AFramePlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}


void AFramePlayerState::OnRep_Score() 
{
    Super::OnRep_Score();

    Character = Character == nullptr ? Cast<AFrameCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<AFramePlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}


void AFramePlayerState::AddToElims(int32 ElimsAmount)
{
    Elims += ElimsAmount;

    Character = Character == nullptr ? Cast<AFrameCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<AFramePlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDElims(Elims);
        }
    }
}


void AFramePlayerState::OnRep_Elims()
{
    Character = Character == nullptr ? Cast<AFrameCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<AFramePlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDElims(Elims);
        }
    }
}


void AFramePlayerState::SetTeam(ETeam TeamToSet)
{
    Team = TeamToSet;

    AFrameCharacter* FCharacter = Cast<AFrameCharacter>(GetPawn());
    if (FCharacter)
    {
        FCharacter->SetTeamColour(Team);
    }
}


void AFramePlayerState::OnRep_Team()
{
    AFrameCharacter* FCharacter = Cast<AFrameCharacter>(GetPawn());
    if (FCharacter)
    {
        FCharacter->SetTeamColour(Team);
    }
}




