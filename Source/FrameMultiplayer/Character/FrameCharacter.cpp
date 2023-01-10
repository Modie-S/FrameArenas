// MaxiMod Games 2022
// Modie Shakarchi


#include "FrameCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "FrameMultiplayer/Weapon/Weapon.h"
#include "FrameMultiplayer/Components/CombatComponent.h"
#include "FrameMultiplayer/Components/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "FrameAnimInstance.h"
#include "FrameMultiplayer/FrameMultiplayer.h"
#include "FrameMultiplayer/PlayerController/FramePlayerController.h"
#include "FrameMultiplayer/GameMode/FrameGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "FrameMultiplayer/PlayerState/FramePlayerState.h"
#include "FrameMultiplayer/Weapon/WeaponTypes.h"
#include "Components/BoxComponent.h"
#include "FrameMultiplayer/Components/LagCompensationComponent.h"


AFrameCharacter::AFrameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh()); // Attached to mesh so camera moves when crouching etc
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("LeftHandSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//
	// Hit Boxes - Server-Side rewind
	//

	Head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	Head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), Head);

	Pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	Pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), Pelvis);

	Spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	Spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), Spine_02);

	Spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	Spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), Spine_03);

	UpperArm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	UpperArm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), UpperArm_l);

	UpperArm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	UpperArm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_2"), UpperArm_r);

	LowerArm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	LowerArm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), LowerArm_l);

	LowerArm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	LowerArm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), LowerArm_r);

	Hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	Hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), Hand_l);

	Hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	Hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), Hand_r);

	Thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	Thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), Thigh_l);

	Thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	Thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), Thigh_r);

	Calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	Calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), Calf_l);

	Calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	Calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), Calf_r);

	Foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	Foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), Foot_l);

	Foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	Foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), Foot_r);	

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
}

void AFrameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFrameCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AFrameCharacter, Health);
	DOREPLIFETIME(AFrameCharacter, Shield);
	DOREPLIFETIME(AFrameCharacter, bDisableGameplay);
}

void AFrameCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AFrameCharacter::ReceiveDamage);
	}

	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
}

void AFrameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
}

void AFrameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFrameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFrameCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn Right / Left", this, &AFrameCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up / Down", this, &AFrameCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AFrameCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AFrameCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AFrameCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFrameCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AFrameCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AFrameCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFrameCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFrameCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFrameCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AFrameCharacter::ThrowButtonPressed);
}

void AFrameCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<AFramePlayerController>(Controller);
		}
	}
}

void AFrameCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AFrameCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AFrameCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AFrameCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AFrameCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();
		bool bSwap = Combat->CanSwapWeapon() && !HasAuthority() && Combat->CombatState == ECombatState::ECS_Unoccupied && OverlappingWeapon == nullptr;
		if (bSwap)
		{
			PlayWeaponSwapMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishSwapping = false;
		}
	}
}

void AFrameCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->CanSwapWeapon())
		{
			Combat->SwapWeapon();
		}
	}
}

void AFrameCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AFrameCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->Reload();
	}
}

void AFrameCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AFrameCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void AFrameCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AFrameCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AFrameCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleFire") : FName("RifleFire");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AFrameCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
			case EWeaponType::EWT_AssaultRifle:
				SectionName = FName("RifleReload");
				break;
			case EWeaponType::EWT_RocketLauncher:
				SectionName = FName("RocketReload");
				break;
			case EWeaponType::EWT_Pistol:
				SectionName = FName("PistolReload");
				break;
			case EWeaponType::EWT_SubmachineGun:
				SectionName = FName("SMGReload");
				break;
			case EWeaponType::EWT_Shotgun:
				SectionName = FName("ShotgunReload");
				break;
			case EWeaponType::EWT_SniperRifle:
				SectionName = FName("SniperReload");
				break;
			case EWeaponType::EWT_GrenadeLauncher:
				SectionName = FName("GrenadeReload");
				break;
		}	


		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AFrameCharacter::PlayWeaponSwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && WeaponSwapMontage)
	{
		AnimInstance->Montage_Play(WeaponSwapMontage);
	}
}

void AFrameCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void AFrameCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("HitFront");
		
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AFrameCharacter::PlayGrenadeThrowMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GrenadeThrowMontage)
	{
		AnimInstance->Montage_Play(GrenadeThrowMontage);
	}
}

void AFrameCharacter::ThrowButtonPressed()
{
	if (Combat)
	{
		Combat->Throw();
	}
}

void AFrameCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimmed) return;

	float DamageToHealth = Damage;
	if (Shield > 0.f) // Damage shield first
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			Shield = 0.f;
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
		}
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		AFrameGameMode* FrameGameMode = GetWorld()->GetAuthGameMode<AFrameGameMode>();
		if (FrameGameMode)
		{
			FramePlayerController = FramePlayerController == nullptr ? Cast<AFramePlayerController>(Controller) : FramePlayerController;
			AFramePlayerController* AttackerController = Cast<AFramePlayerController>(InstigatorController);
			FrameGameMode->PlayerEliminated(this, FramePlayerController, AttackerController);
		}
	}
	
}

void AFrameCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void AFrameCharacter::UpdateHUDHealth()
{
	FramePlayerController = FramePlayerController == nullptr ? Cast<AFramePlayerController>(Controller) : FramePlayerController;
	if (FramePlayerController)
	{
		FramePlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AFrameCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();
	}
}

void AFrameCharacter::UpdateHUDShield()
{
	FramePlayerController = FramePlayerController == nullptr ? Cast<AFramePlayerController>(Controller) : FramePlayerController;
	if (FramePlayerController)
	{
		FramePlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void AFrameCharacter::UpdateHUDAmmo()
{
	FramePlayerController = FramePlayerController == nullptr ? Cast<AFramePlayerController>(Controller) : FramePlayerController;
	if (FramePlayerController && Combat && Combat->EquippedWeapon)
	{
		FramePlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		FramePlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void AFrameCharacter::SpawnDefaultWeapon()
{
	AFrameGameMode* FrameGameMode = Cast<AFrameGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (FrameGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void AFrameCharacter::PollInit()
{
	if (FramePlayerState == nullptr)
	{
		FramePlayerState = GetPlayerState<AFramePlayerState>();
		if (FramePlayerState)
		{
			FramePlayerState->AddToScore(0.f);
			FramePlayerState->AddToElims(0);
		}
	}
}

void AFrameCharacter::Elim()
{
	DropOrDestroyWeapons();
	
	MulticastElim();
	
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this, 
		&AFrameCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void AFrameCharacter::MulticastElim_Implementation()
{
	if (FramePlayerController)
	{
		FramePlayerController->SetHUDWeaponAmmo(0);
		FramePlayerController->SetHUDWeaponType(FText::FromString(""));
	}
	
	bElimmed = true;
	PlayElimMontage();

	// Starting dissolve effect 
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();

	// Disabling character movement/player input
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}

	// Disabling collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn Elim Drone
	if (ElimDroneEffect)
	{
		FVector ElimDroneSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimDroneComponent = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), 
				ElimDroneEffect,
				ElimDroneSpawnPoint,
				GetActorRotation()
				);
	}

	if (ElimDroneSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimDroneSound,
			GetActorLocation()
			);
	}

	bool bHideSniperScope = IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScope(false);
	}
}

void AFrameCharacter::DropOrDestroy(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void AFrameCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroy(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroy(Combat->SecondaryWeapon);
		}
	}
}

void AFrameCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimDroneComponent)
	{
		ElimDroneComponent->DestroyComponent();
	}

	AFrameGameMode* FrameGameMode = Cast<AFrameGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = FrameGameMode && FrameGameMode->GetMatchState() != MatchState::InProgress;

	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void AFrameCharacter::ElimTimerFinished()
{
	AFrameGameMode* FrameGameMode = GetWorld()->GetAuthGameMode<AFrameGameMode>();
	if (FrameGameMode)
	{
		FrameGameMode->RequestRespawn(this, Controller);
	}
}

void AFrameCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void AFrameCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &AFrameCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void AFrameCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void AFrameCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AFrameCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

bool AFrameCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AFrameCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* AFrameCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector AFrameCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState AFrameCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

bool AFrameCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}