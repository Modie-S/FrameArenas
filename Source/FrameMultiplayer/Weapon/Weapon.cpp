// MaxiMod Games 2023
// Modie Shakarchi

#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "FrameMultiplayer/Character/FrameCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"
#include "FrameMultiplayer/Components/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"


AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TEAL);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	
	
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
 	AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(OtherActor);
	if (FrameCharacter)
	{
		FrameCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFrameCharacter* FrameCharacter = Cast<AFrameCharacter>(OtherActor);
	if (FrameCharacter)
	{
		FrameCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();
}

void AWeapon::OnEquipped()
{
	ShowPickupWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);
}

void AWeapon::OnDropped()
{
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TEAL);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);
}

void AWeapon::OnEquippedSecondary()
{
	ShowPickupWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EnableCustomDepth(false);
}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
		case EWeaponState::EWS_Equipped:
			OnEquipped();
			break;
		case EWeaponState::EWS_EquippedSecondary:
			OnEquippedSecondary();
			break;
		case EWeaponState::EWS_Dropped:
			OnDropped();
			break;
	}
}

void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}

	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("BulletShell_Socket"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
					);
			}
		}
	}
	
	UseRound();
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	FrameOwnerCharacter = nullptr;
	FrameOwnerController = nullptr;
}

void AWeapon::AddAmmo(int32 AmmoAdded)
{
	Ammo = FMath::Clamp(Ammo + AmmoAdded, 0, MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmmoAdded);
}

void AWeapon::UseRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;
	}
}

void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority()) return;
	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence;
	SetHUDAmmo();
}

void AWeapon::ClientAddAmmo_Implementation(int32 AmmoAdded)
{
	if (HasAuthority()) return;
	Ammo = FMath::Clamp(Ammo + AmmoAdded, 0, MagCapacity);
	FrameOwnerCharacter = FrameOwnerCharacter == nullptr ? Cast<AFrameCharacter>(GetOwner()) : FrameOwnerCharacter;
	if (FrameOwnerCharacter && FrameOwnerCharacter->GetCombat() && IsFull())
	{
		FrameOwnerCharacter->GetCombat()->JumpToShotgunEnd();
	}

	SetHUDAmmo();
}

void AWeapon::SetHUDAmmo()
{
	FrameOwnerCharacter = FrameOwnerCharacter == nullptr ? Cast<AFrameCharacter>(GetOwner()) : FrameOwnerCharacter;
	if (FrameOwnerCharacter)
	{
		FrameOwnerController = FrameOwnerController == nullptr ? Cast<AFramePlayerController>(FrameOwnerCharacter->Controller) : FrameOwnerController;
		if (FrameOwnerController)
		{
			FrameOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		FrameOwnerCharacter = nullptr;
		FrameOwnerController = nullptr;
	}
	else
	{
		FrameOwnerCharacter = FrameOwnerCharacter == nullptr ? Cast<AFrameCharacter>(Owner) : FrameOwnerCharacter;
		if (FrameOwnerCharacter && FrameOwnerCharacter->GetEquippedWeapon() && FrameOwnerCharacter->GetEquippedWeapon() == this)
		{
			SetHUDAmmo();
		}
	}	
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

bool AWeapon::IsFull()
{
	return Ammo == MagCapacity;
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

FVector AWeapon::TraceEndScatter(const FVector& HitTarget)
{
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash_Socket_01");
    if (MuzzleFlashSocket == nullptr) return FVector();
    
    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
    const FVector TraceStart = SocketTransform.GetLocation() + 15.f;
    
    const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
    const FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
    const FVector EndLoc = SphereCenter + RandomVector;
    const FVector ToEndLoc = EndLoc - TraceStart;

    /*DrawDebugSphere(
        GetWorld(),
        SphereCenter,
        SphereRadius,
        12,
        FColor::Red,
        true
    );

    DrawDebugSphere(
        GetWorld(),
        EndLoc,
        4.f,
        12,
        FColor::Orange,
        true
    );

    DrawDebugLine(
        GetWorld(),
        TraceStart,
        FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
        FColor::Cyan,
        true
    );*/

    return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}



