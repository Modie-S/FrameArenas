// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUps.generated.h"

UCLASS()
class FRAMEMULTIPLAYER_API APickUps : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APickUps();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult
		);

	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;

private:

	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	class USoundCue* PickUpSound;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickUpMesh;

public:	
	
	

};
