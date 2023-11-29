// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicVolume.h"
#include "SpherePhysicVolume.generated.h"

/**
 * 
 */
UCLASS(Abstract, ClassGroup = (Volumes))
class PHYSICVOLUMES_API ASpherePhysicVolume : public APhysicVolume
{
	GENERATED_BODY()
	
private:
	//Set the height of the floor from the center of the volume. 
	UPROPERTY(EditAnywhere, Category = "PhysicVolumeSettings")
		float GroundLevel;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShapeSettings")
		float SphereRadius;

public:

	ASpherePhysicVolume();
	void OnConstruction(const FTransform& Transform) override;

	virtual FVector GetGravityDirection(AActor*& simulatedObject, bool ShouldCheckAltitude) const override;
	virtual float GetActorAltitude(const AActor* Actor) const override;
	virtual float GetAltitudeAlpha(const AActor* Actor) const override;
	virtual float GetOccupiedVolume() const override;
};
