// Fill out your copyright notice in the Description page of Project Settings.


#include "VolumeConnector.h"

#include "Volumes/PhysicVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void AVolumeConnector::OpenClose()
{
	SetIsOpen(!bIsOpen);
}

// Sets default values
AVolumeConnector::AVolumeConnector()
	: BoxHalfExtent(50.0f, 50.0f, 50.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UBoxComponent* Volume = CreateDefaultSubobject<UBoxComponent>("Volume");
	Volume->SetGenerateOverlapEvents(true);
	Volume->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Volume->SetBoxExtent(BoxHalfExtent);
	RootComponent = Volume;

}

AVolumeConnector::AVolumeConnector(FVector BoxExtent)
	: BoxHalfExtent(BoxExtent)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UBoxComponent* Volume = CreateDefaultSubobject<UBoxComponent>("Volume");
	Volume->SetBoxExtent(BoxHalfExtent);
	RootComponent = Volume;
}

void AVolumeConnector::OnConstruction(const FTransform& Transform)
{
	((UBoxComponent*)RootComponent)->SetBoxExtent(BoxHalfExtent);
}

void AVolumeConnector::SetupConnector()
{
	const TArray<TEnumAsByte<EObjectTypeQuery>> Query = { (TEnumAsByte<EObjectTypeQuery>)1, (TEnumAsByte<EObjectTypeQuery>)3 };
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OverlappedActors;
	TArray<APhysicVolume*> ChildVolumes;
	TSubclassOf<APhysicVolume> VolumeClass;
	if (UKismetSystemLibrary::ComponentOverlapActors((UBoxComponent*)RootComponent, this->GetTransform(), Query, VolumeClass, ActorsToIgnore, OverlappedActors))
	{
		for (int i = 0; i != OverlappedActors.Num(); ++i)
		{
			if (!FirstVolume)
			{
				FirstVolume = (APhysicVolume*)OverlappedActors[i];
				FirstVolume->AddConnector(this);
				OnDestroyed.AddDynamic(this, &AVolumeConnector::RemoveFromParentVolume);
				if (!GetAttachParentActor()) 
				{
					AttachToActor(FirstVolume, FAttachmentTransformRules{EAttachmentRule::KeepWorld, false});
				}
				continue;
			}
			if (!SecondVolume)
			{
				SecondVolume = (APhysicVolume*)OverlappedActors[i];
				continue;
			}
			
			if (FirstVolume->GetPriority() < ((APhysicVolume*)OverlappedActors[i])->GetPriority())
			{
				FirstVolume = (APhysicVolume*)OverlappedActors[i];
			}
			else if (SecondVolume->GetPriority() < ((APhysicVolume*)OverlappedActors[i])->GetPriority())
			{
				SecondVolume = (APhysicVolume*)OverlappedActors[i];
			}
		}
	}

	/*if (FirstVolume && SecondVolume)
	{
		FString firstName = UKismetSystemLibrary::GetDisplayName(FirstVolume);
		FString secondName = UKismetSystemLibrary::GetDisplayName(SecondVolume);
		UE_LOG(LogTemp, Warning, TEXT("First Volume: %s."), *firstName);
		UE_LOG(LogTemp, Warning, TEXT("Second Volume: %s."), *secondName);
	}*/
}

void AVolumeConnector::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto newVolume = Cast<APhysicVolume>(OtherActor);
	if (newVolume)
	{
		if(newVolume == FirstVolume)
		{
			return;
		}

		if (!SecondVolume) 
		{
			SecondVolume = newVolume;
			return;
		}
		else
		{
			if (SecondVolume->GetPriority() <= newVolume->GetPriority())
			{
				SecondVolume = newVolume;
				return;
			}
		}

		/*if (!FirstVolume)
		{
			FirstVolume = newVolume;
			if (bIsOpen && SecondVolume)
			{
				FirstVolume->UpdateConnectedVolume(SecondVolume, bIsOpen);
				SecondVolume->UpdateConnectedVolume(FirstVolume, bIsOpen);
			}
			return;
		}
		if (!SecondVolume)
		{
			SecondVolume = newVolume;
			if (bIsOpen && FirstVolume)
			{
				FirstVolume->UpdateConnectedVolume(SecondVolume, bIsOpen);
				SecondVolume->UpdateConnectedVolume(FirstVolume, bIsOpen);
			}
			return;
		}

		if (newVolume->GetPriority() > FirstVolume->GetPriority())
		{
			FirstVolume->UpdateConnectedVolume(SecondVolume, false);
			SecondVolume->UpdateConnectedVolume(FirstVolume, false);
			FirstVolume = newVolume;
			FirstVolume->UpdateConnectedVolume(SecondVolume, bIsOpen);
			SecondVolume->UpdateConnectedVolume(FirstVolume, bIsOpen);
			return;
		}
		if (newVolume->GetPriority() > SecondVolume->GetPriority())
		{
			FirstVolume->UpdateConnectedVolume(SecondVolume, false);
			SecondVolume->UpdateConnectedVolume(FirstVolume, false);
			SecondVolume = newVolume;
			FirstVolume->UpdateConnectedVolume(SecondVolume, bIsOpen);
			SecondVolume->UpdateConnectedVolume(FirstVolume, bIsOpen);
			return;
		}*/
	}
}

void AVolumeConnector::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto newVolume = Cast<APhysicVolume>(OtherActor);
	if (newVolume)
	{
		if (newVolume == FirstVolume)
			return;

		if (newVolume == SecondVolume)
			SecondVolume = SecondVolume->GetParentVolume();
	}

	/*if (FirstVolume && OtherActor == FirstVolume)
	{
		if (SecondVolume)
		{
			FirstVolume->UpdateConnectedVolume(SecondVolume, false);
			SecondVolume->UpdateConnectedVolume(FirstVolume, false);
		}
		FirstVolume = nullptr;
		return;
	}
	if (SecondVolume && OtherActor == SecondVolume)
	{
		if (FirstVolume)
		{
			FirstVolume->UpdateConnectedVolume(SecondVolume, false);
			SecondVolume->UpdateConnectedVolume(FirstVolume, false);
		}
		SecondVolume = nullptr;
		return;
	}*/
}

void AVolumeConnector::RemoveFromParentVolume(AActor* Act)
{
	if (FirstVolume)
	{
		FirstVolume->RemoveConnector(this);
	}

}

// Called when the game starts or when spawned
void AVolumeConnector::BeginPlay()
{
	Super::BeginPlay();

	SetupConnector();

	((UBoxComponent*)RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &AVolumeConnector::BeginOverlap);
	((UBoxComponent*)RootComponent)->OnComponentEndOverlap.AddDynamic(this, &AVolumeConnector::EndOverlap);
	
}

// Called every frame
void AVolumeConnector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVolumeConnector::SetIsOpen(bool newOpen)
{
	bIsOpen = newOpen;

	if (FirstVolume && SecondVolume)
	{
		//FirstVolume->OnConnectorUpdate.Broadcast(bIsOpen, SecondVolume);
		FirstVolume->UpdateConnectedVolume(SecondVolume, bIsOpen);
		SecondVolume->UpdateConnectedVolume(FirstVolume, bIsOpen);
	}

	OnConnectorOpened.Broadcast(bIsOpen);
}

