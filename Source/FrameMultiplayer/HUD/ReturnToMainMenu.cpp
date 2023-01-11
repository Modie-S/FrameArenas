// MaxiMod Games 2023
// Modie Shakarchi

#include "ReturnToMainMenu.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "MPSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"


void UReturnToMainMenu::MenuSetup()
{
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld* World = GetWorld();
    if (World)
    {
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        if (PlayerController)
        {
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }

    if (ReturnButton && !ReturnButton->OnClicked.IsBound())
    {
        ReturnButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
    }

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMPSessionsSubsystem>();
        if (MultiplayerSessionsSubsystem)
        {
            MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UReturnToMainMenu::OnDestroySession);
        }
    }
}

bool UReturnToMainMenu::Initialize()
{
    if (!Super::Initialize())
    {
        return false;
    }

    return true;
}

void UReturnToMainMenu::OnDestroySession(bool bWasSuccessful)
{
    if (!bWasSuccessful)
    {
        ReturnButton->SetIsEnabled(true);
        return;
    }
    
    UWorld* World = GetWorld();
    if (World)
    {
        AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
        if (GameMode)
        {
            GameMode->ReturnToMainMenuHost();
        }
        else
        {
            PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
            if (PlayerController)
            {
                PlayerController->ClientReturnToMainMenuWithTextReason(FText());
            }
        }
    }
}

void UReturnToMainMenu::MenuTearDown()
{
    RemoveFromParent();
    UWorld* World = GetWorld();
    if (World)
    {
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        if (PlayerController)
        {
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }

    if (ReturnButton && ReturnButton->OnClicked.IsBound())
    {
        ReturnButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
    }

    if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
    {
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UReturnToMainMenu::OnDestroySession);
    }
}

void UReturnToMainMenu::ReturnButtonClicked()
{
    ReturnButton->SetIsEnabled(false);

    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* FirstPlayerController = World->GetFirstPlayerController();
        if (FirstPlayerController)
        {
            AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(FirstPlayerController->GetPawn());
            if (FrameCharacter)
            {
                FrameCharacter->ServerLeaveGame();
                FrameCharacter->OnLeftGame.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);
            }
            else
            {
                ReturnButton->SetIsEnabled(true);
            }
        }
    }
}

void UReturnToMainMenu::OnPlayerLeftGame()
{
    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->DestroySession();
    }
}