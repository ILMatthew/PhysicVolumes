// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include <algorithm>

#include "PhysicVolume.generated.h"

//UDELEGATE()
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConnectorUpdateDelegate, bool, IsOpen, const APhysicVolume*, otherVolume);

class UPrimitiveComponent;
class AVolumeConnector;
//TMap<UPrimitiveComponent*, APhysicVolume*> APhysicVolume::GlobalMap;

UCLASS(Abstract, ClassGroup = (Volumes))
class PHYSICVOLUMES_API APhysicVolume : public AActor
{
	GENERATED_BODY()
	
private:

	///////// MEMBERS ////////////

	//Usaed to indicate nesting order. Volumes with lower priority are container of those with heigher ones.
	UPROPERTY(EditAnywhere, Category="PhysicVolumeSettings")
		int Priority;

	/** List of simulated actors inside the volume */
	TArray<UPrimitiveComponent*> SimulatedObjetcs;

	APhysicVolume* ParentVolume;
	TArray<APhysicVolume*> ChildVolumes;
	
	/** List of other volumes connected directly or through connectors */
	TArray<APhysicVolume*> ConnectedVolumes;

	TArray<AVolumeConnector*> Connectors;

	/** Map of all simulated objects and volumes that own them */
	static TMap<UPrimitiveComponent*, APhysicVolume*> GlobalMap;

	////////// METHODS ///////////////

	////////// Collisions and Simulated objects authority ///////////////

	UFUNCTION()
		void CaptureSimulatedObject(UPrimitiveComponent* object);

	UFUNCTION()
		void SurrenderSimulatedObject(UPrimitiveComponent* object);

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION()
		void OnSimulatedObjectDestroy(AActor* DestroyedActor);
	////////////////////////////////////////////////////////////////////


	//Change priority of the volume when needed if it enters/leaves another volume
	void UpdateVolumeDependency(APhysicVolume* Vol, bool isEntering);

	//Update the priority of this volume and call this function in all child volumes
	void RaisePriority();

protected:
	//Select if this volume should inherith all properties from its parent
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhysicVolumeSettings")
		bool InheritFromParent;

	//Actual shape of the volume. Gets overriden in derived classes
	UPrimitiveComponent* Volume;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	APhysicVolume();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Initial setup executed at begin play
	void SetupVolume();

	/////////////// GETTERS AND SETTERS ////////////////

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physic Volume Settings")
		int GetPriority() const { return Priority; };
	
	//Get the Volume containing the current one.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physic Volume Settings")
		APhysicVolume* GetParentVolume() const { return ParentVolume; };
	
	//Get a list of all Volumes contained in this one.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physic Volume Settings")
		const TArray<APhysicVolume*>& GetChildVolumes() const { return ChildVolumes; };
	
	//Get a list of all connected volumes, either by connectors or by overlapping.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physic Volume Settings")
		const TArray<APhysicVolume*>& GetConnetedVolumes() const { return ConnectedVolumes; };
	
	//Connect or disconnect a volume from this one.
	UFUNCTION(BlueprintCallable, Category = "Physic Volume Settings")
		void UpdateConnectedVolume(APhysicVolume* otherVolume, bool isConnected);

	//Get a list of all connecters attached to this volume.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physic Volume Settings")
		const TArray<AVolumeConnector*>& GetConnectors() const { return Connectors; };

	//Add a new connector to this volume.
	UFUNCTION(BlueprintCallable, Category = "Physic Volume Settings")
		void AddConnector(AVolumeConnector* newConnector) { Connectors.AddUnique(newConnector); }

	//Remove a connector to this volume.
	UFUNCTION(BlueprintCallable, Category = "Physic Volume Settings")
		void RemoveConnector(AVolumeConnector* newConnector) { Connectors.Remove(newConnector); }

	//Get a list of simulated objects inside this Volume.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physic Volume Settings")
		const TArray<UPrimitiveComponent*> GetSimulatedObjects() const { return SimulatedObjetcs; }

	//Get the direction of gravity inside this Volume.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physic Volume Settings") //, BlueprintNativeEvent)
		virtual FVector GetGravityDirection(UPARAM(ref) AActor*& simulatedObject, bool ShouldCheckAltitude) const;

	//Get the amount of space occupied by the volume in cm^3.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physic Volume Settings")
		virtual float GetOccupiedVolume() const { return 0.0f; }

	//Get object altitude inside this Volume in cm.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Simulation")
		virtual float GetActorAltitude(const AActor* Actor) const { return 0.0f; }

	//Get object altitude as alpha (1.0 is ground level, 0.0 is height limit of the volume).
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Simulation")
		virtual float GetAltitudeAlpha(const AActor* Actor) const { return 0.0f; }
	////////////////////////////////////////////////////////////////////


	////////// Collisions and Simulated objects authority ///////////////

	void AddSimulatedObject(UPrimitiveComponent* object);
	void RemoveSimulatedObject(UPrimitiveComponent* object);
	////////////////////////////////////////////////////////////////////


	/////////////// GLOBAL MAP METHODS ////////////////

	//Add an object to the list of simulated objects and/or update its owner volume
	static void SetGlobalMapEntry(UPrimitiveComponent* object, APhysicVolume* objOwner);

	//Remove an object from the list of simulated objects.
	UFUNCTION(BlueprintCallable, Category = "Global Map")
		static void RemoveGlobalMapEntry(const UPrimitiveComponent* object);

	//Get the Volume currently responsable for the given Primitive Component, if any.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Global Map")
		static APhysicVolume* GetOwnerVolume(UPARAM(ref) UPrimitiveComponent*& object);
	////////////////////////////////////////////////////////////////////


	/*//Get the Volume currently responsable for the given actor, if any.
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static APhysicVolume* GetOwnerVolume(const AActor* object);*/

};