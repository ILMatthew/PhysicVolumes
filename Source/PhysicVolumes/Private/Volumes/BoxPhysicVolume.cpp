// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/BoxPhysicVolume.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

ABoxPhysicVolume::ABoxPhysicVolume()
{
	BoxHalfExtent = { 50.0f, 50.0f, 50.0f };
	Volume = CreateDefaultSubobject<UBoxComponent>("Volume");
	((UBoxComponent*)Volume)->SetLineThickness(5.0);
	RootComponent = Volume;
}

void ABoxPhysicVolume::OnConstruction(const FTransform& Transform)
{
	((UBoxComponent*)Volume)->SetBoxExtent(BoxHalfExtent);

	DrawDebugSphere(GetWorld(), GetActorLocation() + GetActorUpVector() * (BoxHalfExtent.Z - GroundLevel) * -1,
		25.0f, 32, FColor(255, 0, 0, 1), false, 0.1f, 0, 1.0f);

}

FVector ABoxPhysicVolume::GetGravityDirection(AActor*& simulatedObject, bool ShouldCheckAltitude) const
{
	FVector GravDir = APhysicVolume::GetGravityDirection(simulatedObject, ShouldCheckAltitude);
	if (!GravDir.IsZero())
		return GravDir;

	float alpha = 1.0f;

	if (ShouldCheckAltitude)
	{
		alpha = GetActorAltitude(simulatedObject) / BoxHalfExtent.Z * 2;
		alpha = std::max<float>(1 - alpha, 0.05f);
	}

	return -GetActorUpVector() * alpha;
}

float ABoxPhysicVolume::GetActorAltitude(const AActor* Actor) const
{
	FVector floorCenter = GetActorLocation() + GetActorUpVector() * (BoxHalfExtent.Z - GroundLevel) * -1;
	FVector floorToObjDir = Actor->GetActorLocation() - floorCenter;
	if (!floorToObjDir.Normalize())
		return 0.0f;

	double dot = floorToObjDir.Dot(GetActorUpVector());
	double angleInRadians = cos((dot * -1.0 + 1.0) * 90.0 / 180.0 * PI);
	return (Actor->GetActorLocation() - floorCenter).Length() * angleInRadians;
}

float ABoxPhysicVolume::GetAltitudeAlpha(const AActor* Actor) const
{
	float altitude = GetActorAltitude(Actor);
	float alpha = std::min<float>(1, altitude / (BoxHalfExtent.Z * 2 - GroundLevel));
	alpha = std::max<float>(0, alpha);
	return 1 - alpha;
}

float ABoxPhysicVolume::GetOccupiedVolume() const
{
	return BoxHalfExtent.X * BoxHalfExtent.Y * BoxHalfExtent.Z * 2.0f;
}
