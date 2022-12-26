#pragma once

#define TRACE_LENGTH 800000.f

#define CUSTOM_DEPTH_TEAL 250
#define CUSTOM_DEPTH_ORANGE 251
#define CUSTOM_DEPTH_WHITE 252


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
    EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
    EWT_Pistol UMETA(DisplayName = "Pistol"),
    EWT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
    EWT_Shotgun UMETA(DisplayName = "Shotgun"),
    EWT_SniperRifle UMETA(DisplayName = "Sniper Rifle"),
    EWT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),


    EWT_MAX UMETA(DisplayName = "DefaultMAX")
};