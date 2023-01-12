// MaxiMod Games 2023
// Modie Shakarchi


#include "KDAnnouncement.h"
#include "Components/TextBlock.h"


void UKDAnnouncement::SetKDAnnouncementText(FString AttackerName, FString VictimName)
{
    FString AnnouncementText = FString::Printf(TEXT("%s > %s"), *AttackerName, *VictimName);
    if (KDAnnouncementText)
    {
        KDAnnouncementText->SetText(FText::FromString(AnnouncementText));
    }
}

