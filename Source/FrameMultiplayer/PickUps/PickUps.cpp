// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "FrameMultiplayer/Weapon/WeaponTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

APickUps::APickUps()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root")); // Root placed on ground when moving pickups on map

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OverlapSphere->AddLocalOffset(FVector(0.f, 0.f, 50.f));

	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUpMesh"));
	PickUpMesh->SetupAttachment(OverlapSphere);
	PickUpMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickUpMesh->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
	PickUpMesh->SetRenderCustomDepth(true);
	PickUpMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_ORANGE);

	PickUpEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupEffectComponent"));
    PickUpEffectComponent->SetupAttachment(RootComponent);

}


void APickUps::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			BindOverlapTimer,
			this,
			&APickUps::BindOverlapTimerFinished,
			BindOverlapTime
			);
	}
}

void APickUps::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void APickUps::BindOverlapTimerFinished()
{
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUps::OnSphereOverlap);
}


void APickUps::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PickUpMesh)
	{
		PickUpMesh->AddWorldRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}

}

void APickUps::Destroyed()
{
	Super::Destroyed();

	if (PickUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickUpSound,
			GetActorLocation()
			);
	}

	if (PickUpEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        this,
        PickUpEffect,
        GetActorLocation(),
        GetActorRotation()
        );
    }
}

