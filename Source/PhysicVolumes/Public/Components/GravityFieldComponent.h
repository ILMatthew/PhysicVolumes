// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicVolumeComponent.h"
#include "GravityFieldComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (PhysicVolume))
class PHYSICVOLUMES_API UGravityFieldComponent : public UPhysicVolumeComponent
{
	GENERATED_BODY()

private:
	void ApplyGravity();

	//Gravity intensity expressed in m/s^2.
	UPROPERTY(EditAnywhere, Category = "Gravity Settings")
		float GravityStrenght;

	//Determins whether the volume gravity should remain static or descrease based on altitude.
	UPROPERTY(EditAnywhere, Category = "Gravity Settings")
		bool EnableGravityBasedOnAltitude;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Get gravity strenght at sea level in m/s^2.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gravity Settings")
		float GetGravityStrenght() const { return GravityStrenght; }

	//Set gravity strenght at sea level in m/s^2.
	UFUNCTION(BlueprintCallable, Category = "Gravity Settings")
		void SetGravityStrenght(float newGravity) { GravityStrenght = newGravity; }

	//Get gravity force vector on Actor location.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gravity")
		FVector GetGravity(AActor* Actor) const;
	
};
