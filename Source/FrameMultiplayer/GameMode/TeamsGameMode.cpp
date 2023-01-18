// MaxiMod Games 2023
// Modie Shakarchi


#include "TeamsGameMode.h"
#include "FrameMultiplayer/GameState/FrameGameState.h"
#include "FrameMultiplayer/PlayerState/FramePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"



ATeamsGameMode::ATeamsGameMode()
{
   bTeamsMatch = true;
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    AFrameGameState* FGameState = Cast<AFrameGameState>(UGameplayStatics::GetGameState(this));
    if (FGameState)
    {
            AFramePlayerState* FPState = NewPlayer->GetPlayerState<AFramePlayerState>();
            if (FPState && FPState->GetTeam() == ETeam::ET_NoTeam)
            {
                if (FGameState->BlueTeam.Num() >= FGameState->RedTeam.Num())
                {
                    FGameState->RedTeam.AddUnique(FPState);
                    FPState->SetTeam(ETeam::ET_RedTeam);
                }
                else
                {
                    FGameState->BlueTeam.AddUnique(FPState);
                    FPState->SetTeam(ETeam::ET_BlueTeam);
                }
            }
    }
}

void ATeamsGameMode::Logout(AController* Exiting)
{
    AFrameGameState* FGameState = Cast<AFrameGameState>(UGameplayStatics::GetGameState(this));
    AFramePlayerState* FPState = Exiting->GetPlayerState<AFramePlayerState>();
    if (FGameState && FPState)
    {
        if (FGameState->RedTeam.Contains(FPState))
        {
            FGameState->RedTeam.Remove(FPState);
        }
        if (FGameState->BlueTeam.Contains(FPState))
        {
            FGameState->BlueTeam.Remove(FPState);
        }
    }
}

void ATeamsGameMode::HandleMatchHasStarted()
{
    Super::HandleMatchHasStarted();

    AFrameGameState* FGameState = Cast<AFrameGameState>(UGameplayStatics::GetGameState(this));
    if (FGameState)
    {
        for (auto PlayerState : FGameState->PlayerArray)
        {
            AFramePlayerState* FPState = Cast<AFramePlayerState>(PlayerState.Get());
            if (FPState && FPState->GetTeam() == ETeam::ET_NoTeam)
            {
                if (FGameState->BlueTeam.Num() >= FGameState->RedTeam.Num())
                {
                    FGameState->RedTeam.AddUnique(FPState);
                    FPState->SetTeam(ETeam::ET_RedTeam);
                }
                else
                {
                    FGameState->BlueTeam.AddUnique(FPState);
                    FPState->SetTeam(ETeam::ET_BlueTeam);
                }
            }
        }
    }
}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
    AFramePlayerState* AttackerPlayerState = Attacker->GetPlayerState<AFramePlayerState>();
    AFramePlayerState* VictimPlayerState = Victim->GetPlayerState<AFramePlayerState>();
    if (AttackerPlayerState == nullptr || VictimPlayerState == nullptr) return BaseDamage;
    if (VictimPlayerState == AttackerPlayerState)
    {
        return BaseDamage;
    }
    if (AttackerPlayerState->GetTeam() == VictimPlayerState->GetTeam())
    {
        return 0.f;
    }
    return BaseDamage;
}

void ATeamsGameMode::PlayerEliminated(AFrameCharacter* ElimmedCharacter, AFramePlayerController* VictimController, AFramePlayerController* AttackerController)
{
    Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

    AFrameGameState* FGameState = Cast<AFrameGameState>(UGameplayStatics::GetGameState(this));
    AFramePlayerState* AttackerPlayerState = AttackerController ? Cast<AFramePlayerState>(AttackerController->PlayerState) : nullptr;
    if (FGameState && AttackerPlayerState)
    {
        if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
        {
            FGameState->BlueTeamScored();
        }
        if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
        {
            FGameState->RedTeamScored();
        }
    }
}
