// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"




AProjectileRocket::AProjectileRocket()
{
    RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
    RocketMesh->SetupAttachment(RootComponent);
    RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
    RocketMovementComponent->bRotationFollowsVelocity = true;
    RocketMovementComponent->SetIsReplicated(true);
}


void AProjectileRocket::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}
    
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
    
    if (ProjectileLoop && LoopSoundAttenuation)
    {
        ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
            ProjectileLoop,
            GetRootComponent(),
            FName(),
            GetActorLocation(),
            EAttachLocation::KeepWorldPosition,
            false,
            1.f,
            1.f,
            0.f,
            LoopSoundAttenuation,
            (USoundConcurrency*)nullptr,
            false
        );
    }
}

void AProjectileRocket::DestroyTimerFinished()
{
    Destroy();
}


void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
                200.f,  // Inner Radius of Damage
                500.f,  // Outer Radius of Damage
                1.f,    // Damage fall off
                UDamageType::StaticClass(), // Damage Type Class
                TArray<AActor*>(),  //IgnoreActors
                this,    // Damage Cuaser
                FiringController // InstigatorController
            );
        }
    }
    
    GetWorldTimerManager().SetTimer(
        DestroyTimer,
        this,
        &AProjectileRocket::DestroyTimerFinished,
        DestroyTime
    );

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

    if (RocketMesh)
    {
        RocketMesh->SetVisibility(false);
    }

    if (CollisionBox)
    {
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (TrailSystemComponent && TrailSystemComponent->GetSystemInstance())
    {
        TrailSystemComponent->GetSystemInstance()->Deactivate();
    }

    if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
    {
        ProjectileLoopComponent->Stop();
    }
}


void AProjectileRocket::Destroyed()
{
    
}

