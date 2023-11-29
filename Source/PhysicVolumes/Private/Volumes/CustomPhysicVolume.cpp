// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/CustomPhysicVolume.h"

#include "Components/StaticMeshComponent.h"

#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ACustomPhysicVolume::ACustomPhysicVolume()
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> FoundMaterial(TEXT("/PhysicVolumes/Materials/MI_CustomVolumeWireframe"));
	if (FoundMaterial.Succeeded())
	{
		wireframeMaterial = FoundMaterial.Object;
	}


	Volume = CreateDefaultSubobject<UStaticMeshComponent>("Volume");
	RootComponent = Volume;
	Volume->SetHiddenInGame(true, false);
	Volume->SetEnableGravity(false);
	Volume->SetCollisionProfileName("OverlapAllDynamic");

	if (wireframeMaterial)
	{
		Volume->SetMaterial(0, wireframeMaterial);
	}

	if (CustomVolume)
	{
		((UStaticMeshComponent*)Volume)->SetStaticMesh(CustomVolume);
	}
}

void ACustomPhysicVolume::OnConstruction(const FTransform& Transform)
{
	if (CustomVolume)
	{
		((UStaticMeshComponent*)Volume)->SetStaticMesh(CustomVolume);
	}

	GravityDirection.Normalize();

	float VolumeHeight = ((UStaticMeshComponent*)RootComponent)->Bounds.BoxExtent.Z;
	DrawDebugSphere(GetWorld(), GetActorLocation() + GetActorUpVector() * (VolumeHeight - GroundLevel) * -1,
		25.0f, 32, FColor(255, 0, 0, 1), false, 0.1f, 0, 1.0f);

	SetActorLocation(Transform.GetLocation());
}

FVector ACustomPhysicVolume::GetCustomGravityDirection_Implementation(UPARAM(ref)AActor*& simulatedObject, bool ShouldCheckAltitude) const
{
	return GravityDirection;
}

FVector ACustomPhysicVolume::GetGravityDirection(AActor*& simulatedObject, bool ShouldCheckAltitude) const
{
	return GetCustomGravityDirection(simulatedObject, ShouldCheckAltitude);
}

float ACustomPhysicVolume::GetActorAltitude(const AActor* Actor) const
{
	float VolumeHeight = ((UStaticMeshComponent*)RootComponent)->Bounds.BoxExtent.Z;
	FVector floorCenter = GetActorLocation() + GetActorUpVector() * (VolumeHeight - GroundLevel) * -1;
	FVector floorToObjDir = Actor->GetActorLocation() - floorCenter;
	if (!floorToObjDir.Normalize())
		return 0.0f;

	double dot = floorToObjDir.Dot(GetActorUpVector());
	double angleInRadians = cos((dot * -1.0 + 1.0) * 90.0 / 180.0 * PI);
	return (Actor->GetActorLocation() - floorCenter).Length() * angleInRadians;
}

float ACustomPhysicVolume::GetAltitudeAlpha(const AActor* Actor) const
{
	float VolumeHeight = ((UStaticMeshComponent*)RootComponent)->Bounds.BoxExtent.Z;
	float altitude = GetActorAltitude(Actor);
	float alpha = std::min<float>(1, altitude / (VolumeHeight * 2 - GroundLevel));
	alpha = std::max<float>(0, alpha);
	return 1 - alpha;
}

float ACustomPhysicVolume::GetOccupiedVolume() const
{
	FBoxSphereBounds bounds = ((UStaticMeshComponent*)RootComponent)->Bounds;

	float boxVolume = bounds.BoxExtent.X * bounds.BoxExtent.Y * bounds.BoxExtent.Z;
	float sphereVolume = PI * bounds.SphereRadius * bounds.SphereRadius * bounds.SphereRadius * (3.0f / 4.0f);

	return boxVolume < sphereVolume ? boxVolume : sphereVolume;
}
