// MaxiMod Games 2023
// Modie Shakarchi



#include "CaptureTheFlagGameMode.h"
#include "FrameMultiplayer/Weapon/Flag.h"
#include "FrameMultiplayer/CaptureTheFlag/FlagZone.h"
#include "FrameMultiplayer/GameState/FrameGameState.h"



void ACaptureTheFlagGameMode::PlayerEliminated(class AFrameCharacter* ElimmedCharacter, class AFramePlayerController* VictimController, AFramePlayerController* AttackerController)
{
    AFrameGameMode::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController); // Using FGM version as kills in CTF do no count towards, team score, only individual scores

}

void ACaptureTheFlagGameMode::FlagCaptured(class AFlag* Flag, class AFlagZone* Zone)
{
    bool bValidCapture = Flag->Team != Zone->Team;
    AFrameGameState* FGameState = Cast<AFrameGameState>(GameState);
    if (FGameState)
    {
        if (Zone->Team == ETeam::ET_BlueTeam)
        {
            FGameState->RedTeamScored();
        }
        if (Zone->Team == ETeam::ET_RedTeam)
        {
            FGameState->BlueTeamScored();
        }
    }
}
