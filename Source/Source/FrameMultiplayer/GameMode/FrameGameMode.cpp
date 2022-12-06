// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameGameMode.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FrameMultiplayer/PlayerState/FramePlayerState.h"

void AFrameGameMode::PlayerEliminated(class AFrameCharacter* ElimmedCharacter, class AFramePlayerController* VictimController, AFramePlayerController* AttackerController)
{
    AFramePlayerState* AttackerPlayerState = AttackerController ? Cast<AFramePlayerState>(AttackerController->PlayerState) : nullptr;
    AFramePlayerState* VictimPlayerState = VictimController ? Cast<AFramePlayerState>(VictimController->PlayerState) : nullptr;

    if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
    {
        AttackerPlayerState->AddToScore(10.f);
    }

    if (VictimPlayerState)
    {
        VictimPlayerState->AddToElims(1);
    }

    if (ElimmedCharacter)
    {
        ElimmedCharacter->Elim();
    }
}


void AFrameGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
    if (ElimmedCharacter)
    {
        ElimmedCharacter->Reset();
        ElimmedCharacter->Destroy();
    }

    if (ElimmedController)
    {
        // Respawn at random locations
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
        int32 RespawnSelection = FMath::RandRange(0, PlayerStarts.Num() - 1);
        RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[RespawnSelection]);
    }
}
