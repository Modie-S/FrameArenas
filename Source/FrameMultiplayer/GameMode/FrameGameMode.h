// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FrameGameMode.generated.h"

namespace MatchState
{
	extern FRAMEMULTIPLAYER_API const FName Cooldown; // Match duration reached, display winner and begin cooldown timer
}

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AFrameGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	AFrameGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AFrameCharacter* ElimmedCharacter, class AFramePlayerController* VictimController, AFramePlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(class AFramePlayerState* PlayerLeaving);
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);


	UPROPERTY(EditDefaultsOnly)
	float WarmUpTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 600.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

	bool bTeamsMatch = false;

protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:

	float CountdownTime = 0.f;

public:

	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
