// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ALobbyGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ALobbyPreview> LobbyPreviewClass = nullptr;

	TArray<FVector> LobbySpawnPositions
		= { FVector(-1680.000003,105.000022,60.000000),
			FVector(-1679.999996,245.000022,60.000000),
			FVector(-1765.000010,-34.999974,65.000000),
			FVector(-1764.999990,385.000026,65.000000) };
};
