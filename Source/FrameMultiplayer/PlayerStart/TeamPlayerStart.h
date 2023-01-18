// MaxiMod Games 2023 
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "FrameMultiplayer/Types/Team.h"
#include "TeamPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API ATeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	ETeam Team;

};
