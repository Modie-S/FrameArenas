// MaxiMod Games 2023
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "FrameGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API ATeamsGameMode : public AFrameGameMode
{
	GENERATED_BODY()

public: 

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;

protected:

	virtual void HandleMatchHasStarted() override;
	
};
