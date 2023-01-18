// MaxiMod Games 2022
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FrameMultiplayer/Types/TurnInPlace.h"
#include "FrameMultiplayer/Interfaces/CrosshairInteractInterface.h"
#include "Components/TimelineComponent.h"
#include "FrameMultiplayer/Types/CombatState.h"
#include "FrameMultiplayer/Types/Team.h"
#include "FrameCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class FRAMEMULTIPLAYER_API AFrameCharacter : public ACharacter, public ICrosshairInteractInterface
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	AFrameCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	
	// Montages
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayWeaponSwapMontage();
	void PlayElimMontage();
	void PlayGrenadeThrowMontage();
	void Elim(bool bPlayerLeftGame);
	
	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void SpawnDefaultWeapon();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim(bool bPlayerLeftGame);
	virtual void Destroyed() override;
	
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScope(bool bShowScope);

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	bool bFinishSwapping = false;
	
	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();
	
	FOnLeftGame OnLeftGame;

	void SetTeamColour(ETeam Team);

	/*UFUNCTION(NetMutlicast, Reliable)
	void MulticastGainedLead();

	UFUNCTION(NetMutlicast, Reliable)
	void MulticastLostLead();*/
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();
	void ThrowButtonPressed();
	void DropOrDestroy(AWeapon* Weapon);
	void DropOrDestroyWeapons();
	void SetSpawnPoint();
	void OnPlayerStateInitialized();
	
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	
	// Poll for relevant classes and initialize HUD
	void PollInit();

	//
	// Hit Boxes used for Server-Side Rewind
	//

	UPROPERTY(EditAnywhere)
	class UBoxComponent* Head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* LowerArm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* LowerArm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Foot_r;

private:

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	//
	// Components
	//
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	//
	// Animation Montages
	//
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* GrenadeThrowMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* WeaponSwapMontage;


	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	//
	// Player Health
	//
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	//
	// Player Shield
	//
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY()
	class AFramePlayerController* FramePlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;
	
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	
	void ElimTimerFinished();

	bool bLeftGame = false;

	//
	// Dissolve FX
	//
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	// Dynamic material that can change during runtime
	UPROPERTY(VisibleAnywhere, Category = Elimination)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on Blueprint - used with dynamic MI
	UPROPERTY(EditAnywhere, Category = Elimination)
	UMaterialInstance* DissolveMaterialInstance;

	//
	// Team Colours
	//
	UPROPERTY(EditAnywhere, Category = Elimination)
	UMaterialInstance* RedDissolveMaterialInst;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* RedTeamMaterial0;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* RedTeamMaterial1;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* RedTeamMaterial2;

	UPROPERTY(EditAnywhere, Category = Elimination)
	UMaterialInstance* BlueDissolveMaterialInst;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* BlueTeamMaterial0;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* BlueTeamMaterial1;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* BlueTeamMaterial2;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* StandardTeamMaterial0;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* StandardTeamMaterial1;

	UPROPERTY(EditAnywhere, Category = Teams)
	UMaterialInstance* StandardTeamMaterial2;

	//
	// Elim FX
	//
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimDroneEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimDroneComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimDroneSound;

	UPROPERTY()
	class AFramePlayerState* FramePlayerState;

	/*UPROPERTY(EditAnywhere)
	UParticleSystem* LeaderSystem;

	UPROPERTY()
	UParticleSystemComponent* LeaderComponent;*/

	//
	// Grenade
	//
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;

	//
	// Default Weapon - player starts with this
	//
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY()
	class AFrameGameMode* FrameGameMode;

public:

	// Reserved space for get and set methods
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; } 
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagComp() const { return LagCompensation; }
	FORCEINLINE bool IsHoldingFlag() const;
	ETeam GetTeam();
	void SetHoldingFlag(bool bHolding);
};

