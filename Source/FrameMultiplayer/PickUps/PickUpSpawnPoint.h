// MaxiMod Games 2022
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpSpawnPoint.generated.h"

UCLASS()
class FRAMEMULTIPLAYER_API APickUpSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APickUpSpawnPoint();
	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickUps>> PickUpClasses;

	UPROPERTY()
	APickUps* SpawnedPickUp;

	void SpawnPickUp();
	void SpawnPickUpTimerFinished();
	
	UFUNCTION()
	void StartSpawnPickUpTimer(AActor* DestroyedActor);

private:

	FTimerHandle SpawnPickUpTimer;

	UPROPERTY(EditAnywhere)
	float SpawnPickUpTimeMin;

	UPROPERTY(EditAnywhere)
	float SpawnPickUpTimeMax;

public:	
	
	

};
