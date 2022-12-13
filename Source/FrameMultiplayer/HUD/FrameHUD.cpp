// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"


void AFrameHUD::BeginPlay()
{
    Super::BeginPlay();
}


void AFrameHUD::AddCharacterOverlay()
{
    APlayerController* PlayerController = GetOwningPlayerController();
    if (PlayerController && CharacterOverlayClass)
    {
        CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
        CharacterOverlay->AddToViewport();
    }

}


void AFrameHUD::AddAnnouncement()
{
    APlayerController* PlayerController = GetOwningPlayerController();
    if (PlayerController && AnnouncementClass && Announcement == nullptr)
    {
        Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
        Announcement->AddToViewport();
    }
}


void AFrameHUD::DrawHUD()
{
    Super::DrawHUD();

    FVector2D ViewportSize;
    if (GEngine)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        const FVector2D ViewportCentre(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

        float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

        if (HUDPackage.CrosshairsCentre)
        {
            FVector2D Spread(0.f, 0.f);
            DrawCrosshair(HUDPackage.CrosshairsCentre, ViewportCentre, Spread, HUDPackage.CrosshairsColor);
        }
        if (HUDPackage.CrosshairsLeft)
        {
            FVector2D Spread(-SpreadScaled, 0.f);
            DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCentre, Spread, HUDPackage.CrosshairsColor);
        }
        if (HUDPackage.CrosshairsRight)
        {
            FVector2D Spread(SpreadScaled, 0.f);
            DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCentre, Spread, HUDPackage.CrosshairsColor);
        }
        if (HUDPackage.CrosshairsTop)
        {
            FVector2D Spread(0.f, -SpreadScaled);
            DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCentre, Spread, HUDPackage.CrosshairsColor);
        }
        if (HUDPackage.CrosshairsBottom)
        {
            FVector2D Spread(0.f, SpreadScaled);
            DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCentre, Spread, HUDPackage.CrosshairsColor);
        }
    }
}

void AFrameHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCentre, FVector2D Spread, FLinearColor CrosshairColor)
{
    const float TextureWidth = Texture->GetSizeX();
    const float TextureHeight = Texture->GetSizeY();
    const FVector2D TextureDrawPoint(
                ViewportCentre.X - (TextureWidth / 2.f) + Spread.X,
                ViewportCentre.Y - (TextureHeight / 2.f) + Spread.Y
                );

    DrawTexture(
            Texture,
            TextureDrawPoint.X,
            TextureDrawPoint.Y,
            TextureWidth,
            TextureHeight,
            0.f,
            0.f,
            1.f,
            1.f,
            CrosshairColor
            );
}
