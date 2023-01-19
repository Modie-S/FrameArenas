// MaxiMod Games 2023
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	

public: 

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
