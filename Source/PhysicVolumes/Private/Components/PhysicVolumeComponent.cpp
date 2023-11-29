// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PhysicVolumeComponent.h"

#include "Volumes/PhysicVolume.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPhysicVolumeComponent::UPhysicVolumeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	auto maybeVolume = Cast<APhysicVolume>(GetOwner());
	if (!maybeVolume) {
		FString ownerName = UKismetSystemLibrary::GetDisplayName(GetOwner());
		//UE_LOG(LogTemp, Error, TEXT("%s is not a Physic Volume. Remove the Atmosphere Component to avoid errors"), *ownerName);
	}
}


// Called when the game starts
void UPhysicVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

	auto maybeVolume = Cast<APhysicVolume>(GetOwner());
	if (!maybeVolume) {
		FString ownerName = UKismetSystemLibrary::GetDisplayName(GetOwner());
		//UE_LOG(LogTemp, Error, TEXT("%s is not a Physic Volume. Remove the Atmosphere Component to avoid errors"), *ownerName);
		DestroyComponent();
	}
	
}


// Called every frame
void UPhysicVolumeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

