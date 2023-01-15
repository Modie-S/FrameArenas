// MaxiMod Games 2023
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FrameMultiplayer/Types/Team.h"
#include "FramePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AFramePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//
	// Rep Notifies
	//
	
	virtual void OnRep_Score() override;
	
	UFUNCTION()
	virtual void OnRep_Elims();
	
	void AddToScore(float ScoreAmount);
	void AddToElims(int32 ElimsAmount);

private:

	UPROPERTY()
	class AFrameCharacter* Character;

	UPROPERTY()
	class AFramePlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Elims)
	int32 Elims;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
	void OnRep_Team();

public:

	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
};
