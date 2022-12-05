// Fill out your copyright notice in the Description page of Project Settings.


#include "FramePlayerController.h"
#include "FrameMultiplayer/HUD/FrameHUD.h"
#include "FrameMultiplayer/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void AFramePlayerController::BeginPlay()
{
    Super::BeginPlay();

    FrameHUD = Cast<AFrameHUD>(GetHUD());
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
}

