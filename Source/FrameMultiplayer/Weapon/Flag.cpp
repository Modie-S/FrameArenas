// MaxiMod Games 2022
// Modie Shakarchi


#include "Flag.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"


AFlag::AFlag()
{
    FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
    SetRootComponent(FlagMesh);
    GetAreaSphere()->SetupAttachment(FlagMesh);
    GetPickupWidget()->SetupAttachment(FlagMesh);
    FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
