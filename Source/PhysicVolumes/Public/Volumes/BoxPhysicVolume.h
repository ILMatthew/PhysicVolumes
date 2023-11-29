// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicVolume.h"
#include "BoxPhysicVolume.generated.h"

/**
 * 
 */
UCLASS(Abstract, ClassGroup = (Volumes))
class PHYSICVOLUMES_API ABoxPhysicVolume : public APhysicVolume
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "ShapeSettings")
		FVector BoxHalfExtent;

	//Set the height of the floor from the bottom of the volume. 
	UPROPERTY(EditAnywhere, Category = "PhysicVolumeSettings")
		float GroundLevel;

public:

	ABoxPhysicVolume();
	void OnConstruction(const FTransform& Transform) override;

	virtual FVector GetGravityDirection(AActor*& simulatedObject, bool ShouldCheckAltitude) const override;
	virtual float GetActorAltitude(const AActor* Actor) const override;
	virtual float GetAltitudeAlpha(const AActor* Actor) const override;
	virtual float GetOccupiedVolume() const override;
};
