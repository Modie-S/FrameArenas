// Fill out your copyright notice in the Description page of Project Settings.


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


void AFramePlayerController::ServerCheckMatchState_Implementation()
{
    AFrameGameMode* GameMode = Cast<AFrameGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        WarmUpTime = GameMode->WarmUpTime;
        MatchTime = GameMode->MatchTime;
        LevelStartingTime = GameMode->LevelStartingTime;
        MatchState = GameMode->GetMatchState();
        ClientJoinMidgame(MatchState, WarmUpTime, MatchTime, LevelStartingTime);

        if (FrameHUD && MatchState == MatchState::WaitingToStart)
        {
            FrameHUD->AddAnnouncement();
        }
    }
}


void AFramePlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float WarmUp, float Match, float StartingTime)
{
    WarmUpTime = WarmUp;
    MatchTime = Match;
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
        bInitializeCharacterOverlay = true;
        HUDHealth = Health;
        HUDMaxHealth = MaxHealth;
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
        bInitializeCharacterOverlay = true;
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
        bInitializeCharacterOverlay = true;
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

    
    uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
    if (CountdownInt != SecondsLeft)
    {
        if (MatchState == MatchState::WaitingToStart) 
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


void AFramePlayerController::PollInit()
{
    if (CharacterOverlay == nullptr)
    {
        if (FrameHUD && FrameHUD->CharacterOverlay)
        {
            CharacterOverlay = FrameHUD->CharacterOverlay;
            if (CharacterOverlay)
            {
                SetHUDHealth(HUDHealth, HUDMaxHealth);
                SetHUDScore(HUDScore);
                SetHUDElims(HUDElims);
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
    float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
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
}


void AFramePlayerController::OnRep_MatchState()
{
    if (MatchState == MatchState::InProgress)
    {
        HandleMatchHasStarted();
    }
}


void AFramePlayerController::HandleMatchHasStarted()
{
    FrameHUD = FrameHUD == nullptr ? Cast<AFrameHUD>(GetHUD()) : FrameHUD;
        if (FrameHUD)
        {
            FrameHUD->AddCharacterOverlay();
            if (FrameHUD->Announcement)
            {
                FrameHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
            }
        }
}