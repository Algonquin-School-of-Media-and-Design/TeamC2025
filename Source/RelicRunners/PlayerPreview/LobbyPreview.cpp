// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPreview.h"

// Sets default values
ALobbyPreview::ALobbyPreview()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ALobbyPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALobbyPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyPreview::SetupFromPlayerState(APlayerState* PS)
{
	// Example: set mesh, nameplate, etc.
	// Cast to your custom PlayerState to get cosmetics
}

