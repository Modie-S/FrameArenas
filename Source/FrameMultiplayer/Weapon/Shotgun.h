// MaxiMod Games 2023
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:

	virtual void FireShotgun(const TArray<FVector_NetQuantize>& HitTargets);

	void ShotgunTraceEndScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets);

private:

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	uint32 NumberOfBullets = 8;
	
};
