


#include "Weapons/Weapon.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MagazineMesh->SetupAttachment(WeaponMesh, "SOCKET_Magazine");
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

