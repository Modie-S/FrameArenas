// MaxiMod Games 2022
// Modie Shakarchi


#include "FramePlayerController.h"
#include "FrameMultiplayer/HUD/FrameHUD.h"
#include "FrameMultiplayer/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "Net/UnrealNetwork.h"
#include "FrameMultiplayer/GameMode/FrameGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "FrameMultiplayer/HUD/Announcement.h"
#include "FrameMultiplayer/Components/CombatComponent.h"
#include "FrameMultiplayer/GameState/FrameGameState.h"
#include "FrameMultiplayer/PlayerState/FramePlayerState.h"
#include "Components/Image.h"


void AFramePlayerController::BeginPlay()
{
    Super::BeginPlay();

    FrameHUD = Cast<AFrameHUD>(GetHUD());
    ServerCheckMatchState();
}

void AFramePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFramePlayerController, MatchState);
}

void AFramePlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SetHUDTime();
    CheckTimeSync(DeltaTime);
    PollInit();

    CheckPing(DeltaTime);
}

void AFramePlayerController::CheckPing(float DeltaTime)
{
    HighPingRunTime += DeltaTime;
    if (HighPingRunTime > CheckPingFrequency)
    {
        PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
        if (PlayerState)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerState->GetPing() * 4: %d"), PlayerState->GetPing() * 4);
            if (PlayerState->GetPing() * 4 > HighPingThreshold) // Compressed time - divided by 4 to get uint8, multiply by 4 to get accurate ping
            {
                HighPingWarning();
                HighPingAnimationRunTime = 0.f;
                ServerReportPingStatus(true);
            }
            else
            {
                ServerReportPingStatus(false);
            }
        }

        HighPingRunTime = 0.f;
    }

    bool HighPingWarningAnimationPlaying = FrameHUD && 
        FrameHUD->CharacterOverlay && 
        FrameHUD->CharacterOverlay->HighPingWarning && 
        FrameHUD->CharacterOverlay->IsAnimationPlaying(FrameHUD->CharacterOverlay->HighPingWarning);
    if (HighPingWarningAnimationPlaying)
    {
        HighPingAnimationRunTime += DeltaTime;
        if (HighPingAnimationRunTime > HighPingWarningDuration)
        {
            StopHighPingWarning();
        }
    }
}

void AFramePlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
    // Is ping too high to use SSR?
    HighPingDelegate.Broadcast(bHighPing);
}

void AFramePlayerController::CheckTimeSync(float DeltaTime)
{
    TimeSyncRunningTime += DeltaTime;
    if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
    {
        ServerRequestServerTime(GetWorld()->GetTimeSeconds());
        TimeSyncRunningTime = 0.f;
    }
}

void AFramePlayerController::HighPingWarning()
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->HighPingIcon && 
                FrameHUD->CharacterOverlay->HighPingWarning;
    
    if (bHUDValid)
    {
        FrameHUD->CharacterOverlay->HighPingIcon->SetOpacity(1.f);
        FrameHUD->CharacterOverlay->PlayAnimation(
                        FrameHUD->CharacterOverlay->HighPingWarning,
                        0.f,
                        5);
    }
}

void AFramePlayerController::StopHighPingWarning()
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->HighPingIcon && 
                FrameHUD->CharacterOverlay->HighPingWarning;
    
    if (bHUDValid)
    {
        FrameHUD->CharacterOverlay->HighPingIcon->SetOpacity(0.f);
        if (FrameHUD->CharacterOverlay->IsAnimationPlaying(FrameHUD->CharacterOverlay->HighPingWarning))
        {
             FrameHUD->CharacterOverlay->StopAnimation(FrameHUD->CharacterOverlay->HighPingWarning);
        }
    }
}

void AFramePlayerController::ServerCheckMatchState_Implementation()
{
    AFrameGameMode* GameMode = Cast<AFrameGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        WarmUpTime = GameMode->WarmUpTime;
        MatchTime = GameMode->MatchTime;
        CooldownTime = GameMode->CooldownTime;
        LevelStartingTime = GameMode->LevelStartingTime;
        MatchState = GameMode->GetMatchState();
        ClientJoinMidgame(MatchState, WarmUpTime, MatchTime, CooldownTime,LevelStartingTime);

        if (FrameHUD && MatchState == MatchState::WaitingToStart)
        {
            FrameHUD->AddAnnouncement();
        }
    }
}

void AFramePlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float WarmUp, float Match, float Cooldown, float StartingTime)
{
    WarmUpTime = WarmUp;
    MatchTime = Match;
    CooldownTime = Cooldown;
    LevelStartingTime = StartingTime;
    MatchState = StateOfMatch;
    OnMatchStateSet(MatchState);
    if (FrameHUD && MatchState == MatchState::WaitingToStart)
    {
        FrameHUD->AddAnnouncement();
    }
}

void AFramePlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(InPawn);
    if (FrameCharacter)
    {
        SetHUDHealth(FrameCharacter->GetHealth(), FrameCharacter->GetMaxHealth());
    }
}

void AFramePlayerController::SetHUDHealth(float Health, float MaxHealth)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->HealthBar && 
                FrameHUD->CharacterOverlay->HealthText;
    
    if (bHUDValid)
    {
        const float HealthPercent = Health / MaxHealth;
        FrameHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
        FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
        FrameHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
    }
    else
    {
        bInitializeHealth = true;
        HUDHealth = Health;
        HUDMaxHealth = MaxHealth;
    }
}

void AFramePlayerController::SetHUDShield(float Shield, float MaxShield)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->ShieldBar && 
                FrameHUD->CharacterOverlay->ShieldText;
    
    if (bHUDValid)
    {
        const float ShieldPercent = Shield / MaxShield;
        FrameHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
        FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
        FrameHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
    }
    else
    {
        bInitializeShield = true;
        HUDShield = Shield;
        HUDMaxShield = MaxShield;
    }
}

void AFramePlayerController::SetHUDScore(float Score)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->ScoreAmount;
    if (bHUDValid)
    {
        FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
        FrameHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
    }
    else
    {
        bInitializeScore = true;
        HUDScore = Score;
    }
}

void AFramePlayerController::SetHUDElims(int32 Elims)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->ElimsAmount;
    if (bHUDValid)
    {
        FString ElimsText = FString::Printf(TEXT("%d"), Elims);
        FrameHUD->CharacterOverlay->ElimsAmount->SetText(FText::FromString(ElimsText));
    }
    else
    {
        bInitializeElims = true;
        HUDElims = Elims;
    }
}

void AFramePlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->WeaponAmmoAmount;
    if (bHUDValid)
    {
        FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
        FrameHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
    }
    else
    {
        bInitializeWeaponAmmo = true;
        HUDWeaponAmmo = Ammo;
    }
}

void AFramePlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->CarriedAmmoAmount;
    if (bHUDValid)
    {
        FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
        FrameHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
    }
    else
    {
        bInitializeCarriedAmmo = true;
        HUDCarriedAmmo = Ammo;
    }
}

void AFramePlayerController::SetHUDWeaponType(FText WeaponType)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->WeaponTypeText;
    if (bHUDValid)
    {
        FrameHUD->CharacterOverlay->WeaponTypeText->SetText(WeaponType);
    }
}

void AFramePlayerController::SetHUDMatchTime(float CountdownTime)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->MatchTimeText;
    if (bHUDValid)
    {
        if (CountdownTime < 0.f)
        {
            FrameHUD->CharacterOverlay->MatchTimeText->SetText(FText());
            return;
        }
        int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
        int32 Seconds = CountdownTime - Minutes * 60;

        FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        FrameHUD->CharacterOverlay->MatchTimeText->SetText(FText::FromString(CountdownText));
    }
}

void AFramePlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    bool bHUDValid = FrameHUD && 
                FrameHUD->Announcement && 
                FrameHUD->Announcement->WarmUpTime;
    if (bHUDValid)
    {
        if (CountdownTime < 0.f)
        {
            FrameHUD->Announcement->WarmUpTime->SetText(FText());
            return;
        }

        int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
        int32 Seconds = CountdownTime - Minutes * 60;

        FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        FrameHUD->Announcement->WarmUpTime->SetText(FText::FromString(CountdownText));
    }
}

void AFramePlayerController::SetHUDTime()
{
    float TimeLeft = 0.f;
    if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmUpTime - GetServerTime() + LevelStartingTime;
    else if (MatchState == MatchState::InProgress) TimeLeft = WarmUpTime + MatchTime - GetServerTime() + LevelStartingTime;
    else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmUpTime + MatchTime - GetServerTime() + LevelStartingTime;
    
    uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

    if (HasAuthority())
    {
        FrameGameMode = FrameGameMode == nullptr ? Cast<AFrameGameMode>(UGameplayStatics::GetGameMode(this)) : FrameGameMode;
        if (FrameGameMode)
        {
            SecondsLeft = FMath::CeilToInt(FrameGameMode->GetCountdownTime() + LevelStartingTime);
        }
    }

    if (CountdownInt != SecondsLeft)
    {
        if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown) 
        {
            SetHUDAnnouncementCountdown(TimeLeft);
        }
        if (MatchState == MatchState::InProgress)
        {
            SetHUDMatchTime(TimeLeft);
        }
    }

    CountdownInt = SecondsLeft;
}

void AFramePlayerController::SetHUDGrenades(int32 Grenades)
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
    bool bHUDValid = FrameHUD && 
                FrameHUD->CharacterOverlay && 
                FrameHUD->CharacterOverlay->GrenadesText;
    if (bHUDValid)
    {
        FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
        FrameHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
    }
    else
    {
        bInitializeGrenades = true;
        HUDGrenades = Grenades;
    }
}

void AFramePlayerController::PollInit()
{
    if (CharacterOverlay == nullptr)
    {
        if (FrameHUD && FrameHUD->CharacterOverlay)
        {
            CharacterOverlay = FrameHUD->CharacterOverlay;
            if (CharacterOverlay)
            {
                if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
                if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
                if (bInitializeScore) SetHUDScore(HUDScore);
                if (bInitializeElims) SetHUDElims(HUDElims);
                if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
                if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
                
                AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(GetPawn());
                if (FrameCharacter && FrameCharacter->GetCombat())
                {
                    if (bInitializeGrenades) SetHUDGrenades(FrameCharacter->GetCombat()->GetGrenades());
                }
            }
        }
    }
}

void AFramePlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
    float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
    ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AFramePlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
    float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
    SingleTripTime = 0.5f * RoundTripTime;
    float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
    ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AFramePlayerController::GetServerTime()
{
    if (HasAuthority()) return GetWorld()->GetTimeSeconds();
    else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AFramePlayerController::ReceivedPlayer()
{
    Super::ReceivedPlayer();
    if (IsLocalController())
    {
        ServerRequestServerTime(GetWorld()->GetTimeSeconds());
    }
}

void AFramePlayerController::OnMatchStateSet(FName State)
{
    MatchState = State;

    if (MatchState == MatchState::InProgress)
    {
        HandleMatchHasStarted();
    }
    else if (MatchState == MatchState::Cooldown)
    {
        HandleCooldown();
    }
    
}

void AFramePlayerController::OnRep_MatchState()
{
    if (MatchState == MatchState::InProgress)
    {
        HandleMatchHasStarted();
    }
    else if (MatchState == MatchState::Cooldown)
    {
        HandleCooldown();
    }
}

void AFramePlayerController::HandleMatchHasStarted()
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
        if (FrameHUD)
        {
            if (FrameHUD->CharacterOverlay == nullptr) FrameHUD->AddCharacterOverlay();
            if (FrameHUD->Announcement)
            {
                FrameHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
            }
        }
}

void AFramePlayerController::HandleCooldown()
{
     FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
        if (FrameHUD)
        {
            FrameHUD->CharacterOverlay->RemoveFromParent();
            bool bHUDValid = FrameHUD->Announcement && 
                FrameHUD->Announcement->AnnouncementText && 
                FrameHUD->Announcement->InfoText;

            if (bHUDValid)
            {
                FrameHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
                FString AnnouncementText("Next Round Starts In:");
                FrameHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

                AFrameGameState* FrameGameState = Cast<AFrameGameState>(UGameplayStatics::GetGameState(this));
                AFramePlayerState* FramePlayerState = GetPlayerState<AFramePlayerState>();
                if (FrameGameState && FramePlayerState)
                {
                    TArray<AFramePlayerState*> TopPlayers = FrameGameState->TopScoringPlayers;
                    FString InfoTextString;
                    if (TopPlayers.Num() == 0)
                    {
                        InfoTextString = FString("No Winners Here...");
                    }
                    else if (TopPlayers.Num() == 1 && TopPlayers[0] == FramePlayerState)
                    {
                        InfoTextString = FString("You Are The Winner!");
                    }
                    else if (TopPlayers.Num() == 1)
                    {
                        InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
                    }
                    else if (TopPlayers.Num() > 1)
                    {
                        InfoTextString = FString("Players Tied For The Win:\n");
                        for (auto TiedPlayer : TopPlayers)
                        {
                            InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
                        }
                    }


                    FrameHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
                }

               
            }
        }
        AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(GetPawn());
        if (FrameCharacter && FrameCharacter->GetCombat())
        {
            FrameCharacter->bDisableGameplay = true;
            FrameCharacter->GetCombat()->FireButtonPressed(false);
        }
}