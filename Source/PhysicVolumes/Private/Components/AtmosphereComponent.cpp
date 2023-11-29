// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AtmosphereComponent.h"

#include "Volumes/PhysicVolume.h"
#include "VolumeConnector.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"

// Sets default values for this component's properties
UAtmosphereComponent::UAtmosphereComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UAtmosphereComponent::SetConnectedVolumesAtmosphere() const
{
	//Make sure connected volumes have same atmosphere
}

void UAtmosphereComponent::SetConnectedAtmospheres(bool isConnectorOpen, const APhysicVolume* otherVolume)
{
	if(isConnectorOpen)
	{
		if (otherVolume)
		{
			UAtmosphereComponent* otherAtmo = otherVolume->GetComponentByClass<UAtmosphereComponent>();
			if (otherAtmo)
			{
				//ConnectedAtmospheres.AddUnique(otherAtmo);
			}
		}
	}
	else
	{
		if (otherVolume)
		{
			UAtmosphereComponent* otherAtmo = otherVolume->GetComponentByClass<UAtmosphereComponent>();
			if (otherAtmo)
			{
				//ConnectedAtmospheres.Remove(otherAtmo);
			}
		}
	}
}

void UAtmosphereComponent::ExchangeAtmosphere()
{
	auto& myConnectors = ((APhysicVolume*)GetOwner())->GetConnectors();

	/*TArray<AActor*> attachedActors;
	if (GetOwner()->GetParentActor())
	{
		GetOwner()->GetAttachedActors(attachedActors);
	}*/

	for (int i = 0; i != myConnectors.Num(); ++i)
	{
		if (!myConnectors[i]->IsOpen())
			continue;

		APhysicVolume* connectedVol = myConnectors[i]->GetConnectedVolume();
		if (!connectedVol)
			continue;

		UAtmosphereComponent* otherAtmo = connectedVol->GetComponentByClass<UAtmosphereComponent>();

		if (!otherAtmo)
			continue;

		//if (atmosphereInfo.Pressure < otherAtmo->GetPressure())
			//continue;

		//UE_LOG(LogTemp, Warning, TEXT("Exchanging atmosphere"));
		float myVol = ((APhysicVolume*)GetOwner())->GetOccupiedVolume();
		
		auto& myConnectedVols = ((APhysicVolume*)GetOwner())->GetConnetedVolumes();
		for (int x = 0; x != myConnectedVols.Num(); ++x)
		{
			myVol += myConnectedVols[x]->GetOccupiedVolume();
		}
		
		float otherVol = connectedVol->GetOccupiedVolume();
		
		auto& otherConnectedVols = connectedVol->GetConnetedVolumes();
		for (int x = 0; x != otherConnectedVols.Num(); ++x)
		{
			otherVol += otherConnectedVols[x]->GetOccupiedVolume();
		}
		

		float myK = myVol * atmosphereInfo.Pressure;
		float otherK = otherVol * otherAtmo->GetPressure();

		// TEMPERATURE

		{
			float myKTemperature = myK * atmosphereInfo.Temperature;
			float otherKTemperature = otherK * otherAtmo->GetTemperature();

			float differential = (myKTemperature + otherKTemperature) / std::max(myK + otherK, 0.1f);

			atmosphereInfo.Temperature = ((differential * myK - myKTemperature) * FApp::GetDeltaTime() + myKTemperature) / std::max(myK, 0.1f);
			for (int x = 0; x != myConnectedVols.Num(); ++x)
			{
				myConnectedVols[x]->GetComponentByClass<UAtmosphereComponent>()->SetTemperature(atmosphereInfo.Temperature);
			}

			otherAtmo->SetTemperature(((differential * otherK - otherKTemperature) * FApp::GetDeltaTime() + otherKTemperature) / std::max(otherK, 0.1f));
			for (int x = 0; x != otherConnectedVols.Num(); ++x)
			{
				otherConnectedVols[x]->GetComponentByClass<UAtmosphereComponent>()->SetTemperature(otherAtmo->GetTemperature());
			}

			//UE_LOG(LogTemp, Warning, TEXT("My temperature: %f"), myK);
			//UE_LOG(LogTemp, Warning, TEXT("Other temperature: %f"), myKTemperature);
		}

		// GASSES

		//float maxParticleExchange;
		//if (attachedActors.Contains(connectedVolumes[i]))
		//{
		//	maxParticleExchange = INFINITY;
		//}
		//else
		//{
		//	maxParticleExchange = /*Connector volume*/125000 * FApp::GetDeltaTime() * powf((std::abs(atmosphereInfo.Pressure - otherAtmo->GetPressure()) * 2 + 1), 3);
		//}

		float maxParticleExchange = /*Connector volume*/125000 * FApp::GetDeltaTime() * powf((std::abs(atmosphereInfo.Pressure - otherAtmo->GetPressure()) * 2 + 1), 3);

		{
			{
				auto otherGassesArray = otherAtmo->GetAirComposition().Array();
				for (int g = 0; g != otherGassesArray.Num(); ++g)
				{
					if (!atmosphereInfo.AirComposition.Contains(otherGassesArray[i].Key))
						SetAirCompositionParameter(otherGassesArray[i].Key, 0.0f);
				}
			}

			auto gassesArray = atmosphereInfo.AirComposition.Array();

			for (int g = 0; g != gassesArray.Num(); ++g)
			{
				if (!otherAtmo->GetAirComposition().Contains(gassesArray[g].Key))
					otherAtmo->SetAirCompositionParameter(gassesArray[g].Key, 0.0f);
				
				float myParticles = myK * gassesArray[g].Value * 0.01f;
				float otherParticles = otherK * otherAtmo->GetAirComposition().FindRef(gassesArray[g].Key) * 0.01f;

				float differential = (myParticles + otherParticles) / (myK + otherK);
				//float maxExchange = 125000 * FApp::GetDeltaTime() * differential * std::powf((std::abs(atmosphereInfo.Pressure - otherAtmo->GetPressure()) * 2 + 1), 3);

				SetAirCompositionParameter(gassesArray[g].Key, (std::clamp(differential * myK - myParticles, -maxParticleExchange, maxParticleExchange) + myParticles) /
					std::max(myK, 0.1f) * 100.0f);
				for (int x = 0; x != myConnectedVols.Num(); ++x)
				{
					myConnectedVols[x]->GetComponentByClass<UAtmosphereComponent>()->SetAirComposition(atmosphereInfo.AirComposition);
				}

				otherAtmo->SetAirCompositionParameter(gassesArray[g].Key, (std::clamp(differential * otherK - otherParticles, -maxParticleExchange, maxParticleExchange) +
					otherParticles) / std::max(myK, 0.1f) * 100.0f);
				for (int x = 0; x != otherConnectedVols.Num(); ++x)
				{
					otherConnectedVols[x]->GetComponentByClass<UAtmosphereComponent>()->SetAirComposition(otherAtmo->GetAirComposition());
				}

				//float totalGas = (gassesArray[g].Value * myK + ConnectedAtmospheres[i]->GetAirComposition().FindRef(gassesArray[g].Key) * otherK) * 0.01f;
			}
		}

		// PRESSURE

		{
			float medianPressure = (atmosphereInfo.Pressure + otherAtmo->GetPressure()) / 2;

			float depressurStrenght = std::clamp((medianPressure - atmosphereInfo.Pressure) * myVol, -maxParticleExchange, maxParticleExchange);

			auto myObj = ((APhysicVolume*)GetOwner())->GetSimulatedObjects();
			for (int x = 0; x != myObj.Num(); ++x)
			{
				FVector dir = myObj[x]->GetComponentLocation() - myConnectors[i]->GetActorLocation();
				float distance = std::max(dir.Length() / 100, 1.0);
				dir.Normalize();
				myObj[x]->AddForce(dir * depressurStrenght * std::min(FApp::GetDeltaTime(), 0.016) * DepressurizationMultiplier / distance);
			}

			auto otherObj = connectedVol->GetSimulatedObjects();
			for (int x = 0; x != otherObj.Num(); ++x)
			{
				FVector dir = myConnectors[i]->GetActorLocation() - otherObj[x]->GetComponentLocation();
				float distance = std::max(dir.Length() / 100, 1.0);
				dir.Normalize();
				otherObj[x]->AddForce(dir * depressurStrenght * std::min(FApp::GetDeltaTime(), 0.016) * DepressurizationMultiplier / distance);
			}

			atmosphereInfo.Pressure = (depressurStrenght / myVol) + atmosphereInfo.Pressure;
			for (int x = 0; x != myConnectedVols.Num(); ++x)
			{
				myConnectedVols[x]->GetComponentByClass<UAtmosphereComponent>()->SetPressure(atmosphereInfo.Pressure);
			}

			otherAtmo->SetPressure((std::clamp((medianPressure - otherAtmo->GetPressure()) * otherVol, -maxParticleExchange, maxParticleExchange) /
				otherVol) + otherAtmo->GetPressure());
			for (int x = 0; x != otherConnectedVols.Num(); ++x)
			{
				otherConnectedVols[x]->GetComponentByClass<UAtmosphereComponent>()->SetPressure(otherAtmo->GetPressure());
			}
		}

	}
}

// Called when the game starts
void UAtmosphereComponent::BeginPlay()
{
	Super::BeginPlay();

	APhysicVolume* tempVolume = Cast<APhysicVolume>(GetOwner());

	ConnectorDelegate.BindUFunction(this, "SetConnectedAtmospheres");
	//tempVolume->OnConnectorUpdate.Add(ConnectorDelegate);

	if (IneritFromParentVolume)
	{
		UAtmosphereComponent* parentAtmo = tempVolume->GetParentVolume()->GetComponentByClass<UAtmosphereComponent>();
		if (parentAtmo)
		{
			atmosphereInfo = parentAtmo->GetAtmosphereInfo();
		}
	}

	auto& connectedVolums = ((APhysicVolume*)GetOwner())->GetConnetedVolumes();
	for (int i = 0; i != connectedVolums.Num(); ++i)
	{
		auto* atmo = connectedVolums[i]->GetComponentByClass<UAtmosphereComponent>();
		if (atmo && atmo->GetPressure() < atmosphereInfo.Pressure)
		{
			atmo->SetPressure(atmosphereInfo.Pressure);
			atmo->SetAirComposition(atmosphereInfo.AirComposition);
			atmo->SetTemperature(atmosphereInfo.Temperature);
		}
	}

}


// Called every frame
void UAtmosphereComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ExchangeAtmosphere();
}

float UAtmosphereComponent::GetPressureOnActor(const AActor* Actor) const
{
	if (!EnablePressureBasedOnHeight)
		return GetPressure();

	return ((APhysicVolume*)GetOwner())->GetAltitudeAlpha(Actor) * atmosphereInfo.Pressure;
}

void UAtmosphereComponent::SetPressure(float newPresasure)
{
	atmosphereInfo.Pressure = newPresasure;
	OnAtmosphereUpdate.ExecuteIfBound();
}

void UAtmosphereComponent::SetAirCompositionParameter(const Gasses& GasToModify, float newValue)
{
	atmosphereInfo.AirComposition.Add(GasToModify, newValue);

	TArray<float> values;
	atmosphereInfo.AirComposition.GenerateValueArray(values);
	float totalPercentage = 0.0f;

	for (const float& value : values)
	{
		totalPercentage += value;
	}

	if (totalPercentage > 1.0f)
	{
		const float scalingFactor = (1 - newValue) / (totalPercentage - newValue);

		TArray<Gasses> gasses;
		atmosphereInfo.AirComposition.GenerateKeyArray(gasses);
		gasses.Remove(GasToModify);

		for (const Gasses& gas : gasses)
		{
			float value = *atmosphereInfo.AirComposition.Find(gas);
			atmosphereInfo.AirComposition.Add(gas, value * scalingFactor);
		}
	}
}

float UAtmosphereComponent::GetTemperatureOnActor(const AActor* Actor) const
{
	if (!EnablePressureBasedOnHeight)
		return GetTemperature();

	return ((APhysicVolume*)GetOwner())->GetAltitudeAlpha(Actor) * atmosphereInfo.Temperature;
}

void UAtmosphereComponent::SetTemperature(float newTemperature)
{
	atmosphereInfo.Temperature = newTemperature;
	OnAtmosphereUpdate.ExecuteIfBound();
}
