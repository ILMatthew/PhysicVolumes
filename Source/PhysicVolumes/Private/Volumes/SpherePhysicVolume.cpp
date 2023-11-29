// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/SpherePhysicVolume.h"

#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

ASpherePhysicVolume::ASpherePhysicVolume()
{
	SphereRadius = 100.0f;

	Volume = CreateDefaultSubobject<USphereComponent>("Volume");
	RootComponent = Volume;
}

void ASpherePhysicVolume::OnConstruction(const FTransform& Transform)
{
	((USphereComponent*)Volume)->SetSphereRadius(SphereRadius);

	DrawDebugSphere(GetWorld(), GetActorLocation(), GroundLevel, 32, FColor(255, 0, 0, 1), false, 0.1f, 0, 1.0f);

}

FVector ASpherePhysicVolume::GetGravityDirection(AActor*& simulatedObject, bool ShouldCheckAltitude) const
{
	FVector GravDir = APhysicVolume::GetGravityDirection(simulatedObject, ShouldCheckAltitude);
	if (!GravDir.IsZero())
		return GravDir;

	GravDir = GetActorLocation() - simulatedObject->GetActorLocation();
	if (GravDir.Normalize())
	{
		if (ShouldCheckAltitude)
		{
			float alpha = std::max<float>((1 - GetActorAltitude(simulatedObject) / (SphereRadius - GroundLevel)), 0.05f);
			GravDir *= alpha;
		}

		return GravDir;
	}

	return FVector(0.0f, 0.0f, 0.0f);
}

float ASpherePhysicVolume::GetActorAltitude(const AActor* Actor) const
{
	return (Actor->GetActorLocation() - GetActorLocation()).Length() - GroundLevel;
}

float ASpherePhysicVolume::GetAltitudeAlpha(const AActor* Actor) const
{
	float altitude = GetActorAltitude(Actor);
	float alpha = std::min<float>(1, altitude / (SphereRadius - GroundLevel));
	alpha = std::max<float>(0, alpha);
	return 1 - alpha;
}

float ASpherePhysicVolume::GetOccupiedVolume() const
{
	return PI * SphereRadius * SphereRadius * SphereRadius * (3.0f/4.0f);
}
