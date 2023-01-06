// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class FRAMEMULTIPLAYER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:

	AProjectileRocket();

	virtual void Destroyed() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
#endif
	
protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;

	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* LoopSoundAttenuation;

	UPROPERTY(EditAnywhere)
	class URocketMovementComponent* RocketMovementComponent;


private:

	
	



};
