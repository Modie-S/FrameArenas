// MaxiMod Games 2023
// Modie Shakarchi


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"
#include "FrameMultiplayer/Components/LagCompensationComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"




AProjectileBullet::AProjectileBullet()
{
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->SetIsReplicated(true);
    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
    Super::PostEditChangeProperty(Event);

    FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
    {
        if (ProjectileMovementComponent)
        {
            ProjectileMovementComponent->InitialSpeed = InitialSpeed;
            ProjectileMovementComponent->MaxSpeed = InitialSpeed;
        }
    }
}
#endif

void AProjectileBullet::BeginPlay()
{
    Super::BeginPlay();

    /*
    FPredictProjectilePathParams PathParams;
    PathParams.bTraceWithChannel = true;
    PathParams.bTraceWithCollision = true;
    PathParams.DrawDebugTime = 5.f;
    PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
    PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
    PathParams.MaxSimTime = 4.f;
    PathParams.ProjectileRadius = 5.f;
    PathParams.SimFrequency = 30.f;
    PathParams.StartLocation = GetActorLocation();
    PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
    PathParams.ActorsToIgnore.Add(this);

    FPredictProjectilePathResult PathResult;

    UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);*/
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    AFrameCharacter* OwnerCharacter = Cast<AFrameCharacter>(GetOwner());
    if (OwnerCharacter)
    {
        AFramePlayerController* OwnerController = Cast<AFramePlayerController>(OwnerCharacter->Controller);
        if (OwnerController)
        {
            if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
            {
                
                const float DamageToCause = Hit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
                
                UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, OwnerController, this, UDamageType::StaticClass());
                Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
                return;
            }

            AFrameCharacter* HitCharacter = Cast<AFrameCharacter>(OtherActor);
            if (bUseServerSideRewind && OwnerCharacter->GetLagComp() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
            {
                OwnerCharacter->GetLagComp()->ProjectileServerScoreRequest(
                    HitCharacter,
                    TraceStart,
                    InitialVelocity,
                    OwnerController->GetServerTime() - OwnerController->SingleTripTime
                    );
            }           
        }
    }
    
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);   
}



