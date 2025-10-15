/************************************************************************************
 * Author:   Tristan Anglin
 * Year:     2025
 *
 * Notice:
 *   This source code is the intellectual property of Tristan Anglin.
 *   It is provided solely for use within:
 *     - The Relic Runners personal project
 *     - The Almost A Dozen project
 *
 *   Any use, distribution, or modification outside of these projects
 *   is strictly prohibited without explicit written permission.
 *
 *   © 2025 Tristan Anglin. All rights reserved.
 ************************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RelicRunnersPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class RELICRUNNERS_API ARelicRunnersPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WalkAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	/** Inventory Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InventoryAction;

	/** Inventory Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AbilitySelectionAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Basic attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BasicAttackAction;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPossess(APawn* aPawn) override;

	virtual void ClientRestart_Implementation(APawn* NewPawn) override;

	class ARelicRunnersCharacter* GetPossessedPawn() { return PossessedPawn; }

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerName(const FString& NewName);

	UFUNCTION(Server, Reliable)
	void Server_SetSelectedClass(FName NewClass);

	virtual void OnRep_Pawn() override;

	virtual void AcknowledgePossession(APawn* Pawn) override;

	virtual void OnUnPossess() override;

	UFUNCTION()
	void SetupPreviewRenderTarget(APlayerPreview* Preview);

	UFUNCTION()
	void TrySetupPreviewRenderTarget();

	UFUNCTION()
	void SetupLobbyView();

	void SetupMainMenuView();

	UFUNCTION(Server, Reliable)
	void Server_RequestPickup(class AItemActor* Item);

	UPROPERTY(ReplicatedUsing = OnRep_PlayerPreview)
	class APlayerPreview* PlayerPreviewInstance;

	UFUNCTION()
	void OnRep_PlayerPreview();

	UPROPERTY(ReplicatedUsing = OnRep_LobbyPreview)
	class ALobbyPreview* LobbyPreviewInstance;

	UFUNCTION()
	void OnRep_LobbyPreview();

	UFUNCTION(Server, Reliable)
	void Server_RequestStartGame();

	UFUNCTION(Client, Reliable)
	void Client_LeaveSession();

	UFUNCTION()
	void UpdatePreviewWithEquippedItems();

	void UpdatePreviewItemVisual(UObject* MeshAsset, const FString& ItemType);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UJoinUserWidget> LobbyWidgetClass = nullptr;

	UPROPERTY()
	class UJoinUserWidget* LobbyWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass = nullptr;

	UPROPERTY()
	class UMainMenuWidget* MainMenuWidget = nullptr;

	UFUNCTION(Client, Reliable)
	void Client_SetupLobby();

	UFUNCTION(Client, Reliable)
	void Client_UpdateLobbyUI();


	UFUNCTION(Client, Reliable)
	void ClientTravelToGame();

	// Begin Actor interface
protected:
	ARelicRunnersPlayerController();
	virtual void BeginPlay() override;

	void InitializePawnDependentSystems();

	virtual void SetupInputComponent() override;

	void Walk(const struct FInputActionValue& Value);
	void Run(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void Interact();
	void Jump(const struct FInputActionValue& Value);
	void StopJumping(const struct FInputActionValue& Value);
	void InventoryUI();

	void AbilitySystemUI();


	void BasicAttack();

	// End Actor interface

	class ARelicRunnersCharacter* PossessedPawn;


};
