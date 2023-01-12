// MaxiMod Games 2023
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KDAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API UKDAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetKDAnnouncementText(FString AttackerName, FString VictimName);

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* KDAnnouncementBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KDAnnouncementText;
	
};
