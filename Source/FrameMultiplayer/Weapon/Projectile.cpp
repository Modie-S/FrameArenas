// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/FrameMultiplayer.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
					Tracer,
					CollisionBox,
					FName(),
					GetActorLocation(),
					GetActorRotation(),
					EAttachLocation::KeepWorldPosition
					);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void AProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
    {
        TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            TrailSystem,
            GetRootComponent(),
            FName(),
            GetActorLocation(),
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition,
            false
        );
    }
}

void AProjectile::ExplosionDamage()
{
	APawn* FiringPawn = GetInstigator();
    if (FiringPawn && HasAuthority())
    {
        AController* FiringController = FiringPawn->GetController();
        if (FiringController)
        {
            UGameplayStatics::ApplyRadialDamageWithFalloff(
                this,   // World Contect Obj
                Damage, // Base Damage
                10.f,   // Min Damage
                GetActorLocation(), // Origin
                DamageInnerRadius,  // Inner Radius of Damage
                DamageOuterRadius,  // Outer Radius of Damage
                1.f,    // Damage fall off
                UDamageType::StaticClass(), // Damage Type Class
                TArray<AActor*>(),  //IgnoreActors
                this,    // Damage Cuaser
                FiringController // InstigatorController
            );
        }
    }
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::StartDestroyTimer()
{
	 GetWorldTimerManager().SetTimer(
        DestroyTimer,
        this,
        &AProjectile::DestroyTimerFinished,
        DestroyTime
    );
}

void AProjectile::DestroyTimerFinished()
{
    Destroy();
}


void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					GetActorTransform()
					);
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
					this,
					ImpactSound,
					GetActorLocation()
					);
	}

}

