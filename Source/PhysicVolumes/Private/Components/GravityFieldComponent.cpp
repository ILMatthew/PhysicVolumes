// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GravityFieldComponent.h"

#include "Volumes/PhysicVolume.h"
#include "Components/PrimitiveComponent.h"

// Called when the game starts
void UGravityFieldComponent::BeginPlay()
{
	Super::BeginPlay();

	APhysicVolume* tempVolume = Cast<APhysicVolume>(GetOwner());
	if (tempVolume)
	{
		auto& objs = tempVolume->GetSimulatedObjects();
		for (int i = 0; i != objs.Num(); ++i)
		{
			objs[i]->SetEnableGravity(false);
		}
	}
}

// Called every frame
void UGravityFieldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyGravity();
}

FVector UGravityFieldComponent::GetGravity(AActor* Actor) const
{
	FVector gravityDir = ((APhysicVolume*)GetOwner())->GetGravityDirection(Actor, EnableGravityBasedOnAltitude);
	gravityDir *= GravityStrenght;
	return gravityDir;
}

void UGravityFieldComponent::ApplyGravity()
{
	//UE_LOG(LogTemp, Warning, TEXT("Simulating Gravity"));

	auto& objs = ((APhysicVolume*)GetOwner())->GetSimulatedObjects();

	for (int i = 0; i != objs.Num(); ++i)
	{
		AActor* object = objs[i]->GetOwner();
		FVector forceVector = (((APhysicVolume*)GetOwner())->GetGravityDirection(object, EnableGravityBasedOnAltitude) * (GravityStrenght * 100));// +FVector(0.0f, 0.0f, 980.0f);
		objs[i]->AddForce(forceVector, "NAME_None", true);
	}
}