// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/PhysicVolume.h"

#include "Components/PrimitiveComponent.h"
//#include "Components/ShapeComponent.h"
#include "VolumeConnector.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"

TMap<UPrimitiveComponent*, APhysicVolume*> APhysicVolume::GlobalMap;

#pragma region UNREAL EVENTS

// Sets default values
APhysicVolume::APhysicVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APhysicVolume::BeginPlay()
{
	Super::BeginPlay();

	if (Priority == 0)
		SetupVolume();

	//if (Volume == nullptr) { return;}
	Volume->OnComponentBeginOverlap.AddDynamic(this, &APhysicVolume::BeginOverlap);
	Volume->OnComponentEndOverlap.AddDynamic(this, &APhysicVolume::EndOverlap);

}

// Called every frame
void APhysicVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma endregion

void APhysicVolume::CaptureSimulatedObject(UPrimitiveComponent* object)
{
	//UE_LOG(LogTemp, Warning, TEXT("object has entered"));

	auto* tempVol = GetOwnerVolume(object);
	if (tempVol && tempVol->GetPriority() > Priority)
		return;

	SimulatedObjetcs.AddUnique(object);
	SetGlobalMapEntry(object, this);

	//Check if the object is in a connected volume
	for (int i = 0; i != ConnectedVolumes.Num(); ++i)
	{
		if (ConnectedVolumes[i]->GetSimulatedObjects().Contains(object))
			return;
	}

	if (ParentVolume)
		ParentVolume->RemoveSimulatedObject(object);
}

void APhysicVolume::SurrenderSimulatedObject(UPrimitiveComponent* object)
{
	//UE_LOG(LogTemp, Warning, TEXT("object has leaved"));
	if (!object)
		return;

	auto* tempVol = GetOwnerVolume(object);
	if (tempVol != this)
		return;

	SimulatedObjetcs.Remove(object);

	//Check if the object is in a connected volume
	for (int i = 0; i != ConnectedVolumes.Num(); ++i)
	{
		if (ConnectedVolumes[i]->GetSimulatedObjects().Contains(object))
			return;
	}

	if (ParentVolume)
	{
		ParentVolume->AddSimulatedObject(object);
		SetGlobalMapEntry(object, ParentVolume);
		return;
	}

	RemoveGlobalMapEntry(object);
}

void APhysicVolume::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AVolumeConnector>(OtherActor))
		return;
	
	APhysicVolume* maybeVolume = Cast<APhysicVolume>(OtherActor);
	if (maybeVolume)
	{
		UpdateVolumeDependency(maybeVolume, true);
		return;
	}
	CaptureSimulatedObject(OtherComp);
}

void APhysicVolume::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AVolumeConnector>(OtherActor))
		return;

	APhysicVolume* maybeVolume = Cast<APhysicVolume>(OtherActor);
	if (maybeVolume)
	{
		UpdateVolumeDependency(maybeVolume, false);
		return;
	}
	SurrenderSimulatedObject(OtherComp);
}

void APhysicVolume::UpdateVolumeDependency(APhysicVolume* Vol, bool isEntering)
{
	if (Vol->Priority > Priority)
	{
		if (isEntering)
		{
			APhysicVolume* otherParent = Vol->GetParentVolume();
			if (otherParent)
			{
				if (otherParent && otherParent->Priority <= Priority)
				{
					Vol->ParentVolume->ChildVolumes.Remove(Vol);
					Vol->ParentVolume = this;
					ChildVolumes.AddUnique(Vol);
				}
				else if(otherParent)
				{
					APhysicVolume* nestedParent = otherParent;
					for (int i = otherParent->Priority; i != 0; --i)
					{
						if (nestedParent->ParentVolume == this)
							return;

						if (!nestedParent->ParentVolume)
							break;

						nestedParent = nestedParent->ParentVolume;
					}

					Vol->ParentVolume->ChildVolumes.Remove(Vol);
					Vol->ParentVolume = this;
					ChildVolumes.AddUnique(Vol);
				}
			}
			else
			{
				Vol->ParentVolume = this;
				ChildVolumes.AddUnique(Vol);
			}
		}
		else
		{
			if (Vol->ParentVolume == this)
			{
				Vol->ParentVolume = ParentVolume;
				if(Vol->ParentVolume)
					Vol->Priority = Vol->ParentVolume->Priority + 1;

				ChildVolumes.Remove(Vol);
			}
		}
	}
	else if (Vol->Priority == Priority && GetOccupiedVolume() > Vol->GetOccupiedVolume())
	{
		Vol->ParentVolume = this;
		ChildVolumes.AddUnique(Vol);

		Vol->RaisePriority();
	}

}

void APhysicVolume::RaisePriority()
{
	Priority++;
	for (int i = 0; i != ChildVolumes.Num(); ++i)
	{
		ChildVolumes[i]->RaisePriority();
	}
}

void APhysicVolume::OnSimulatedObjectDestroy(AActor* DestroyedActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("Removing object from global map"));
	auto obj = Cast<UPrimitiveComponent>(DestroyedActor->GetRootComponent());

	if (obj) 
	{
		SimulatedObjetcs.Remove(obj);
		for (int i = 0; i != ConnectedVolumes.Num(); ++i)
		{
			ConnectedVolumes[i]->RemoveSimulatedObject(obj);
		}
		RemoveGlobalMapEntry(obj);
	}
}

void APhysicVolume::UpdateConnectedVolume(APhysicVolume* otherVolume, bool isConnected)
{
	/*if (isConnected)
	{
		ConnectedVolumes.AddUnique(otherVolume);
	}
	else
	{
		ConnectedVolumes.Remove(otherVolume);
	}*/
}

void APhysicVolume::SetupVolume()
{
	const TArray<TEnumAsByte<EObjectTypeQuery>> Query = { (TEnumAsByte<EObjectTypeQuery>)1, (TEnumAsByte<EObjectTypeQuery>)3 };
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OverlappedActors;
	//TArray<APhysicVolume*> ChildVolumes;
	if (UKismetSystemLibrary::ComponentOverlapActors(Volume, this->GetTransform(), Query, nullptr, ActorsToIgnore, OverlappedActors))
	{
		for (int i = 0; i != OverlappedActors.Num(); ++i)
		{
			APhysicVolume* tempVolume = dynamic_cast<APhysicVolume*, AActor>(OverlappedActors[i]);
			if (tempVolume)
			{
				int priorityDiff = Priority - tempVolume->GetPriority();

				switch (priorityDiff)
				{
				//Lower Priority - Parent Volume //
				case 1:
					ParentVolume = tempVolume;
					break;
				//Same Priority - Connected Volume //
				case 0:
					ConnectedVolumes.AddUnique(tempVolume);
					break;
				//Heigher Priority - Child Volume //
				case -1:
					ChildVolumes.AddUnique(tempVolume);
					break;
				default:
					break;
				}
			}
			else
			{
				UPrimitiveComponent* tempPrimitive = dynamic_cast<UPrimitiveComponent*, USceneComponent>(OverlappedActors[i]->GetRootComponent());
				if (tempPrimitive)
				{
					SimulatedObjetcs.AddUnique(tempPrimitive);
					SetGlobalMapEntry(tempPrimitive, this);
				}
			}
		}

		if (ParentVolume)
		{
			for (int i = 0; i != SimulatedObjetcs.Num(); ++i)
			{
				ParentVolume->RemoveSimulatedObject(SimulatedObjetcs[i]);
			}
		}

		for (int i = 0; i != ChildVolumes.Num(); ++i)
		{
			ChildVolumes[i]->SetupVolume();
		}
	}
}

void APhysicVolume::SetGlobalMapEntry(UPrimitiveComponent* object, APhysicVolume* objOwner)
{
	//UE_LOG(LogTemp, Warning, TEXT("imposto la globL MAP"));
	APhysicVolume* oldOwner = GetOwnerVolume(object);
	if(oldOwner)
		object->GetOwner()->OnDestroyed.RemoveDynamic(oldOwner, &APhysicVolume::OnSimulatedObjectDestroy);

	GlobalMap.Add(object, objOwner);
	object->GetOwner()->OnDestroyed.AddDynamic(objOwner, &APhysicVolume::OnSimulatedObjectDestroy);
		
	/*APhysicVolume* test = *GlobalMap.Find(object);
	if(test)
		UE_LOG(LogTemp, Warning, TEXT("GlobalMap: %s"), *APhysicVolume::GetDebugName(test));*/
}

void APhysicVolume::RemoveGlobalMapEntry(const UPrimitiveComponent* object)
{
	GlobalMap.Remove(object);
}

APhysicVolume* APhysicVolume::GetOwnerVolume(UPARAM(ref) UPrimitiveComponent*& object)
{
	APhysicVolume** pointer = GlobalMap.Find(object);
	if (!pointer)
		return nullptr;
	
	return *pointer;
}

/*APhysicVolume* APhysicVolume::GetOwnerVolume(const AActor* object)
{
	UPrimitiveComponent* primitiveComp = Cast<UPrimitiveComponent>(object->GetRootComponent());
	if (!primitiveComp)
		return nullptr;

	APhysicVolume** pointer = GlobalMap.Find(primitiveComp);
	if (!pointer)
		return nullptr;
	
	return *pointer;
}*/

void APhysicVolume::AddSimulatedObject(UPrimitiveComponent* object)
{
	SimulatedObjetcs.AddUnique(object);
}

void APhysicVolume::RemoveSimulatedObject(UPrimitiveComponent* object)
{
	SimulatedObjetcs.Remove(object);
}

FVector APhysicVolume::GetGravityDirection(UPARAM(ref) AActor*& simulatedObject, bool ShouldCheckAltitude) const
{
	if (InheritFromParent)
		return ParentVolume->GetGravityDirection(simulatedObject, ShouldCheckAltitude);

	return FVector(0.0f, 0.0f, 0.0f);
}
