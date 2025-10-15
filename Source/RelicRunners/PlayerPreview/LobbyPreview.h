// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyPreview.generated.h"

UCLASS()
class RELICRUNNERS_API ALobbyPreview : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALobbyPreview();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/** Called only on the server to assign which player this preview represents */
	void SetupFromPlayerState(class ARelicRunnersPlayerState* PS);

	/** Reacts to replicated LinkedPlayerState changes */
	UFUNCTION()
	void OnRep_LinkedPlayerState();

	/** Reacts when that player’s class changes (called via delegate) */
	void OnPlayerClassChanged(FName NewClass);

	/** Replication setup */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* LobbyHUDWorld;

	/** Which player this preview belongs to (replicated) */
	UPROPERTY(ReplicatedUsing = OnRep_LinkedPlayerState)
	class ARelicRunnersPlayerState* LinkedPlayerState;
};
