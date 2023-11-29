// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicVolume.h"
#include "CustomPhysicVolume.generated.h"

class UArrowComponent;

/**
 * 
 */
UCLASS(Abstract, ClassGroup = (Volumes))
class PHYSICVOLUMES_API ACustomPhysicVolume : public APhysicVolume
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "ShapeSettings")
		UStaticMesh* CustomVolume;

	//Set the height of the floor from the bottom of the volume. 
	UPROPERTY(EditAnywhere, Category = "PhysicVolumeSettings")
		float GroundLevel;

	UMaterialInterface* wireframeMaterial;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicVolumeSettings")
		FVector GravityDirection;

public:

	ACustomPhysicVolume();
	void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gravity")
		FVector GetCustomGravityDirection(UPARAM(ref) AActor*& simulatedObject, bool ShouldCheckAltitude) const;

	virtual FVector GetGravityDirection(AActor*& simulatedObject, bool ShouldCheckAltitude) const override;
	virtual float GetActorAltitude(const AActor* Actor) const override;
	virtual float GetAltitudeAlpha(const AActor* Actor) const override;
	virtual float GetOccupiedVolume() const override;
};
