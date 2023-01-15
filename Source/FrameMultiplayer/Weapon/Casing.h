// MaxiMod Games 2022
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class FRAMEMULTIPLAYER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ACasing();

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ShellMesh;

	UPROPERTY(EditAnywhere)
	float ShellEjectImpulse;

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

public:	


};
