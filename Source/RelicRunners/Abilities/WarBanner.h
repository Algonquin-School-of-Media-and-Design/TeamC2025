// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Components/TimelineComponent.h"
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "WarBanner.generated.h"
//
//UCLASS()
//class RELICRUNNERS_API AWarBanner : public AActor
//{
//	GENERATED_BODY()
//	
//public:	
//	AWarBanner();
//
//	virtual void BeginPlay() override;
//	virtual void Tick(float DeltaTime) override;
//
//	void Initialize(AActor* newOwner);
//
//	UFUNCTION(Server, Reliable)
//	void Server_Spawn(float Duration, float DamageAmount, float AreaRadius);
//
//	UFUNCTION(Server, Reliable)
//	void Server_Despawn();
//
//	UPROPERTY(EditAnywhere)
//	class UStaticMeshComponent* BannerMesh;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
//	UTimelineComponent* BannerTimelineComp;
//
//	UPROPERTY(EditAnywhere)
//	UCurveFloat* BannerTimelineFloatCurve;
//
//	UPROPERTY(Replicated)
//	bool IsAlive = false;
//
//	UPROPERTY(Replicated)
//	float LifeTime = 0.0f;
//
//	UPROPERTY(Replicated)
//	float Damage = 0.0f;
//
//	UPROPERTY(Replicated)
//	float Radius = 0.0f;
//
//
//private:
//	UFUNCTION()
//	void UpdateTimelineComp(float Output);
//};
