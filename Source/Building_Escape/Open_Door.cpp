// Fill out your copyright notice in the Description page of Project Settings.


#include "Open_Door.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Math/Rotator.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/AudioComponent.h"

// Sets default values for this component's properties
UOpen_Door::UOpen_Door()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	// ...
}


// Called when the game starts
void UOpen_Door::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it,but no pressure plate !!"), *GetOwner()->GetName());
	}
	FindPressurePlate();
	FindAudioComponent();
}

void UOpen_Door::FindPressurePlate()
{
}

void UOpen_Door::FindAudioComponent()
{
	DoorSound = GetOwner()->FindComponentByClass<UAudioComponent>();
}


// Called every frame
void UOpen_Door::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassO() > MassToOpen)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds()- DoorLastOpened>DoorCloseDelay)
		{

		CloseDoor(DeltaTime);
		}
	}

}

void UOpen_Door::OpenDoor(float DeltaTime)
{
	if (!DoorSound) { return; }
	CLD = false;
	if (!OPD)
	{
		UE_LOG(LogTemp, Error, TEXT("OPD = %d"), OPD);
		DoorSound->Play();
		OPD = true;
	}
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime*DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

void UOpen_Door::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
	if (!DoorSound) { return; }
	OPD = false;
	if (!CLD)
	{
		UE_LOG(LogTemp, Error, TEXT("CLD = %d"), CLD);
		DoorSound->Play();
		CLD = true;
	}
}

float UOpen_Door::TotalMassO() const
{
	float TotalMass = 0.f;

	// Find All Overlapping Actors.
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OverlappingActors);

	// Add Up Their Masses.

	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}