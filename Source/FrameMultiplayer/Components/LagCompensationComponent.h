// MaxiMod Games 2023
// Modie Shakarchi

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"


USTRUCT(BlueprintType)
struct FBoxInformation // Information for particular box for given time
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};


USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;

	UPROPERTY()
	AFrameCharacter* Character;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirm;

	UPROPERTY()
	bool bHeadShot;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<AFrameCharacter*, uint32> HeadShots;

	UPROPERTY()
	TMap<AFrameCharacter*, uint32> BodyShots;
	
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRAMEMULTIPLAYER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	ULagCompensationComponent();
	friend class AFrameCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, const FColor Color);
	
	
	// HitScan Weapons
	FServerSideRewindResult ServerSideRewind(
		class AFrameCharacter* HitCharacter, 
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize HitLocation, 
		float HitTime
		);
	
	// Shotgun
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<AFrameCharacter*>& HitCharacters, 
		const FVector_NetQuantize& TraceStart, 
		const TArray<FVector_NetQuantize>& HitLocations, 
		float HitTime
		);
	
	// Projectile Weapons
	FServerSideRewindResult ProjectileServerSideRewind(
		AFrameCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
		);



	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		AFrameCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime,
		class AWeapon* DamageCauser
		);

	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(
		const TArray<AFrameCharacter*>& HitCharacters, 
		const FVector_NetQuantize& TraceStart, 
		const TArray<FVector_NetQuantize>& HitLocations, 
		float HitTime
		);

	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		AFrameCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
		);

protected:
	
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	
	FFramePackage InterpBetweenFrames(
			const FFramePackage& OlderFrame, 
			const FFramePackage& YoungerFrame, 
			float HitTime
			);

	void CacheBoxPositions(AFrameCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AFrameCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AFrameCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AFrameCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(AFrameCharacter* HitCharacter, float HitTime);
	
	//
	// HitScan Weapon
	//
	
	FServerSideRewindResult ConfirmHit(
			const FFramePackage& Package, 
			AFrameCharacter* HitCharacter, 
			const FVector_NetQuantize& TraceStart, 
			const FVector_NetQuantize& HitLocation
			);
	
	//
	// Projectile
	//
	
	FServerSideRewindResult ProjectileConfirmHit(
			const FFramePackage& Package, 
			AFrameCharacter* HitCharacter, 
			const FVector_NetQuantize& TraceStart, 
			const FVector_NetQuantize100& InitialVelocity,
			float HitTime
			);
	
	//
	// Shotgun SSR
	//

	FShotgunServerSideRewindResult ShotgunConfirmHit(
			const TArray<FFramePackage>& FramePackages,
			const FVector_NetQuantize& TraceStart,
			const TArray<FVector_NetQuantize>& HitLocations
			);
	

private:

	UPROPERTY()
	AFrameCharacter* Character;

	UPROPERTY()
	class AFramePlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;

public:	
	
	

		
};
