// MaxiMod Games 2022
// Modie Shakarchi

#include "PickUpSpawnPoint.h"
#include "PickUps.h"


APickUpSpawnPoint::APickUpSpawnPoint()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}


void APickUpSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnPickUpTimer((AActor*)nullptr); // C-Style cast; required to use here otherwise never used
	
}

void APickUpSpawnPoint::SpawnPickUp()
{
	int32 NumPickUpClasses = PickUpClasses.Num();
	if (NumPickUpClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumPickUpClasses - 1);
		SpawnedPickUp = GetWorld()->SpawnActor<APickUps>(PickUpClasses[Selection], GetActorTransform());
	
		if (HasAuthority() && SpawnedPickUp)
		{
			SpawnedPickUp->OnDestroyed.AddDynamic(this, &APickUpSpawnPoint::StartSpawnPickUpTimer);
		}
	
	}
}

void APickUpSpawnPoint::SpawnPickUpTimerFinished()
{
	if (HasAuthority())
	{
		SpawnPickUp();
	}
}

void APickUpSpawnPoint::StartSpawnPickUpTimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnPickUpTimeMin, SpawnPickUpTimeMax);
	GetWorldTimerManager().SetTimer(
		SpawnPickUpTimer,
		this,
		&APickUpSpawnPoint::SpawnPickUpTimerFinished,
		SpawnTime
		);
}


void APickUpSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

