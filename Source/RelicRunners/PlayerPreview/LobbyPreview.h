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
	ALobbyPreview();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void SetupFromPlayerState(class ARelicRunnersPlayerState* PS);

	UFUNCTION()
	void OnRep_LinkedPlayerState();

	void OnPlayerClassChanged(FName NewClass);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* LobbyHUDWorld;

	UPROPERTY(ReplicatedUsing = OnRep_LinkedPlayerState)
	class ARelicRunnersPlayerState* LinkedPlayerState;
};
