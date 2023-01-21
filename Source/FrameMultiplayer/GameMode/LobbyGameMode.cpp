// MaxiMod Games 2023
// Modie Shakarchi


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "MPSessionsSubsystem.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        UMPSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMPSessionsSubsystem>();
        check(Subsystem);

        if (NumberOfPlayers == Subsystem->RequiredNumPublicConnections)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                bUseSeamlessTravel = true;

                FString MatchType = Subsystem->ChosenMatchType;
                if (MatchType == "FreeForAll")
                {
                    World->ServerTravel(FString("/Game/Maps/FreeForAllMAIN?listen")); 
                }
                else if (MatchType == "Teams")
                {
                    World->ServerTravel(FString("/Game/Maps/TDMMain?listen"));
                }
                else if (MatchType == "CaptureTheFlag")
                {
                    World->ServerTravel(FString("/Game/Maps/CTFMain?listen"));
                }

                
            }
        }

    }

   
}
