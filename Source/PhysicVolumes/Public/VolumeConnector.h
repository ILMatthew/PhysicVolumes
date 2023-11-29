// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VolumeConnector.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectorOpenDelegate, bool, IsOpen);

class APhysicVolume;

UCLASS()
class PHYSICVOLUMES_API AVolumeConnector : public AActor
{
	GENERATED_BODY()
	
private:
	APhysicVolume* FirstVolume;
	APhysicVolume* SecondVolume;

	void SetupConnector();

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
		void RemoveFromParentVolume(AActor* Act);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connector Settings")
		FVector BoxHalfExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connector Settings")
		bool bIsOpen;

	UFUNCTION(CallInEditor)
		void OpenClose();

public:	
	// Sets default values for this actor's properties
	AVolumeConnector();
	AVolumeConnector(FVector BoxExtent);

	void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
		FOnConnectorOpenDelegate OnConnectorOpened;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Connector Settings")
		bool IsOpen() { return bIsOpen; }

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Connector Settings")
		void SetIsOpen(bool newOpen);

	UFUNCTION(BlueprintCallable, BlueprintPure, CallInEditor, Category = "Connector Settings")
		APhysicVolume* GetConnectedVolume() const { return SecondVolume; }

};
