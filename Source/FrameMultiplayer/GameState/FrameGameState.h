// MaxiMod Games 2023
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FrameGameState.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AFrameGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AFramePlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<AFramePlayerState*> TopScoringPlayers;

	//
	// Teams
	//

	void RedTeamScored();
	void BlueTeamScored();

	TArray<AFramePlayerState*> RedTeam;
	TArray<AFramePlayerState*> BlueTeam;
	
	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore = 0.f;

	UFUNCTION()
	void OnRep_RedTeamScore();

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;

	UFUNCTION()
	void OnRep_BlueTeamScore();

private:

	float TopScore = 0.f;




};
