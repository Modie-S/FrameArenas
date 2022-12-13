// Modie Shakarchi
// MaxiMod Games 2022


#include "FrameGameMode.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FrameMultiplayer/PlayerState/FramePlayerState.h"

namespace MatchState
{
    const FName Cooldown = FName("Cooldown");
}


AFrameGameMode::AFrameGameMode()
{
    bDelayedStart = true;
}


void AFrameGameMode::BeginPlay()
{
    Super::BeginPlay();

    LevelStartingTime = GetWorld()->GetTimeSeconds();

}


void AFrameGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (MatchState == MatchState::WaitingToStart)
    {
        CountdownTime = WarmUpTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            StartMatch();
        }
    }
    else if (MatchState == MatchState::InProgress)
    {
        CountdownTime = WarmUpTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            SetMatchState(MatchState::Cooldown);
        }
    }
    else if (MatchState == MatchState::Cooldown)
    {
        CountdownTime = CooldownTime + WarmUpTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            RestartGame();
        }
    }
}


void AFrameGameMode::OnMatchStateSet()
{
    Super::OnMatchStateSet();

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AFramePlayerController* FramePlayer = Cast<AFramePlayerController>(*It);
        if (FramePlayer)
        {
            FramePlayer->OnMatchStateSet(MatchState);
        }
    }
}


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

