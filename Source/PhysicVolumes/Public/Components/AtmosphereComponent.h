// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicVolumeComponent.h"
#include "AtmosphereComponent.generated.h"

DECLARE_DELEGATE(FOnAtmosphereUpdateDelegate);

class APhysicVolume;

UENUM(BlueprintType)
enum class Gasses : uint8
{
	Empty = 0 UMETA(DisplayName = " "),
	Hydrogen = 1 UMETA(DisplayName = "Hydrogen"),
	Oxigen = 2 UMETA(DisplayName = "Oxigen"),
	CarbonMonoxide = 3 UMETA(DisplayName = "Carbon Monoxide")
};

USTRUCT(BlueprintType)
struct FAtmosphereInfo
{
	GENERATED_BODY()

public:
	//Air pressure at sea level expressed in bar.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Settings")
		float Pressure;

	//Gas composition. Keys are all the gasses present in game, values are percentage expressed from 0 to 1.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Settings")
		TMap<Gasses, float> AirComposition;

	//The current temperature of the room.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Settings")
		float Temperature;
};

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (PhysicVolume))
class PHYSICVOLUMES_API UAtmosphereComponent : public UPhysicVolumeComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UAtmosphereComponent();

private:
	const float DepressurizationMultiplier = 350.0f;

	TScriptDelegate<FWeakObjectPtr> ConnectorDelegate;

	void SetConnectedVolumesAtmosphere() const;

	void ExchangeAtmosphere();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Settings")
		FAtmosphereInfo atmosphereInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Settings")
		bool EnablePressureBasedOnHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Settings")
		bool IneritFromParentVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Settings")
		bool PressureIsImmutable;

	UFUNCTION()
	void SetConnectedAtmospheres(bool isConnectorOpen, const APhysicVolume* otherVolume);

public:

	FOnAtmosphereUpdateDelegate OnAtmosphereUpdate;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Get pressure at sea level in bar.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Atmosphere Settings")
		float GetPressure() const { return atmosphereInfo.Pressure; }

	//Get pressure on Actor location in bar.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Simulation")
		float GetPressureOnActor(const AActor* Actor) const;

	//Set pressure at sea level in bar.
	UFUNCTION(BlueprintCallable, Category = "Atmosphere Settings")
		void SetPressure(float newPresasure);

	//Get a list of all gasses that compose the atmosphere.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Atmosphere Settings")
		const TMap<Gasses, float>& GetAirComposition() const { return atmosphereInfo.AirComposition; }

	//Set air composition by passing a list of all gasses and relative percentage present in the atmosphere.
	UFUNCTION(BlueprintCallable, Category = "Atmosphere Settings")
		void SetAirComposition(const TMap<Gasses, float>& newAirComposition) { atmosphereInfo.AirComposition = newAirComposition; }

	//Change air composition by setting a new percentage of a gas present in the atmosphere.
	//If total percentage goes above 100%, gasses percentage will be automatically adjasted.
	UFUNCTION(BlueprintCallable, Category = "Atmosphere Settings")
		void SetAirCompositionParameter(const Gasses& GasToModify, float newValue);

	//Get temperature at max pressure in Celsius.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Atmosphere Settings")
		float GetTemperature() const { return atmosphereInfo.Temperature; }

	//Get temperature at Actor location in Celsius.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Simulation")
		float GetTemperatureOnActor(const AActor* Actor) const;

	//Set temperature at sea level in bar.
	UFUNCTION(BlueprintCallable, Category = "Atmosphere Settings")
		void SetTemperature(float newTemperature);

	const FAtmosphereInfo& GetAtmosphereInfo() const { return atmosphereInfo; }
};
