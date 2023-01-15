// MaxiMod Games 2023
// Modie Shakarchi


#include "FrameHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"
#include "KDAnnouncement.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"


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


void AFrameHUD::AddKDAnnouncement(FString Attacker, FString Victim)
{
    OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;
    if (OwningPlayer && KDAnnouncementClass)
    {
        UKDAnnouncement* ElimAnnouncementWidget = CreateWidget<UKDAnnouncement>(OwningPlayer, KDAnnouncementClass);
        if (ElimAnnouncementWidget)
        {
            ElimAnnouncementWidget->SetKDAnnouncementText(Attacker, Victim);
            ElimAnnouncementWidget->AddToViewport();

            for (UKDAnnouncement* Text : KDMessages)
            {
                if (Text && Text->KDAnnouncementBox)
                {
                    UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Text->KDAnnouncementBox);
                    if (CanvasSlot)
                    {
                        FVector2D Position = CanvasSlot->GetPosition();
                        FVector2D NewPosition(CanvasSlot->GetPosition().X, Position.Y - CanvasSlot->GetSize().Y);
                        CanvasSlot->SetPosition(NewPosition);
                    } 
                }
            }

            KDMessages.Add(ElimAnnouncementWidget);

            FTimerHandle KDTextTimer;
            FTimerDelegate KDTextDelegate;
            KDTextDelegate.BindUFunction(this, FName("KDAnnouncementTimerFinished"), ElimAnnouncementWidget);
            GetWorldTimerManager().SetTimer(
                KDTextTimer,
                KDTextDelegate,
                KDAnnouncementLifetime,
                false
                );
        }
    }
}


void AFrameHUD::KDAnnouncementTimerFinished(UKDAnnouncement* TextToRemove)
{
    if (TextToRemove)
    {
        TextToRemove->RemoveFromParent();
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
