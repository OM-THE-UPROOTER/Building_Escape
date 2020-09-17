// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}
 

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandel();
	SetupInputComponent();
	// ...
	
}

void UGrabber::FindPhysicsHandel()
{
	PhysicsHandel = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandel)
	{

	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("No Component Found in %s"),*(GetOwner()->GetName()));
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		UE_LOG(LogTemp,Warning,TEXT("Input component exists"));

			InputComponent->BindAction("Grabber",IE_Pressed,this,&UGrabber::Grab);
			InputComponent->BindAction("Grabber", IE_Repeat, this, &UGrabber::Release);
	}
}

FVector UGrabber::LTE() const
{
	FVector PlayeViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayeViewPointLocation, PlayerViewPointRotation);

	return  PlayeViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp,Warning,TEXT("pickup key is Pressed"));


	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	if (HitResult.GetActor())
	{
		PhysicsHandel->GrabComponentAtLocation(  ComponentToGrab,NAME_None,LTE());
	}

}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("pickup key is Released"));
	PhysicsHandel->ReleaseComponent();

}



// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (PhysicsHandel->GrabbedComponent)
	{
		PhysicsHandel->SetTargetLocation(LTE());
	}
}


FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{

	FVector PlayeViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayeViewPointLocation, PlayerViewPointRotation);


	FCollisionQueryParams TracePrams(FName(TEXT("")),false,GetOwner());
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType
	(
		Hit,
		PlayeViewPointLocation,
		LTE(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TracePrams
	);

	AActor* ActorHit = Hit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp,Error,TEXT("The Actor Has Hit %s"), *(ActorHit->GetName()));
	}
	return Hit;
}