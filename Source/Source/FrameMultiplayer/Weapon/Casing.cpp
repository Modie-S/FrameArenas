// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ACasing::ACasing()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMesh"));
	SetRootComponent(ShellMesh);
	ShellMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	ShellMesh->SetSimulatePhysics(true);
	ShellMesh->SetEnableGravity(true);
	ShellMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectImpulse = 10.f;

}


void ACasing::BeginPlay()
{
	Super::BeginPlay();

	ShellMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	ShellMesh->AddImpulse(GetActorForwardVector() * ShellEjectImpulse);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
					this,
					ShellSound,
					GetActorLocation()
					);
	}
	Destroy();
}


