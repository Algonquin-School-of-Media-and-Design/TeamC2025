// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsWidget.h"
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "KeybindingsListData.h"
#include <InputMappingContext.h>
#include <EnhancedInputSubsystems.h>
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USettingsWidget::BackButtonClicked);
	}

	InitializeDefaultKeybindings();

	if (KeybindingsTileView)
	{
		KeybindingsTileView->ClearListItems();

		for (UKeybindingsListData* Binding : DefaultKeybindings)
		{
			KeybindingsTileView->AddItem(Binding);
		}
	}
}

void USettingsWidget::SetParentMenu(UMainMenuWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}

void USettingsWidget::BackButtonClicked()
{
	if (ParentMenu)
	{
		ParentMenu->ShowModeSelectionWidget();
	}
}

void USettingsWidget::InitializeDefaultKeybindings()
{
	DefaultKeybindings.Empty();

	auto AddBinding = [&](const FString& ActionName, const FKey& Key)
		{
			UKeybindingsListData* NewEntry = NewObject<UKeybindingsListData>(this);
			NewEntry->Name = ActionName;
			NewEntry->Keybind = Key.GetFName().ToString();
			DefaultKeybindings.Add(NewEntry);
		};

	AddBinding("Walk Forward", EKeys::W);
	AddBinding("Walk Left", EKeys::A);
	AddBinding("Walk Backward", EKeys::S);
	AddBinding("Walk Right", EKeys::D);
	AddBinding("Inventory", EKeys::I);
	AddBinding("Ability Selection", EKeys::U);
	AddBinding("Interact", EKeys::E);
	AddBinding("Map", EKeys::M);
	AddBinding("Pause", EKeys::Escape);
	AddBinding("Ping", EKeys::MiddleMouseButton);
	AddBinding("Damage Ability", EKeys::Q);
	AddBinding("Utility Ability", EKeys::R);
	AddBinding("Defence Ability", EKeys::T);
	AddBinding("Ultimate Ability", EKeys::F);
	AddBinding("Health Potion", EKeys::H);

	ApplyKeybindings();
}

void USettingsWidget::ApplyKeybindings()
{

	// Get the player input subsystem
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ARelicRunnersPlayerController* RPC = Cast<ARelicRunnersPlayerController>(PC);
	if (!RPC) return;

	UInputMappingContext* MappingContext = RPC->GetInputMappingContext();
	if (!MappingContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyKeybindings: No InputMappingContext set!"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem) return;

	MappingContext->UnmapAll();

	for (UKeybindingsListData* Binding : DefaultKeybindings)
	{
		if (!Binding) continue;

		// Build action asset name (your naming convention)
		FString ActionAssetName = FString::Printf(TEXT("IA_%s"), *Binding->Name.Replace(TEXT(" "), TEXT("")));
		FString AssetPath = FString::Printf(TEXT("/Game/ThirdPerson/Input/Actions/%s.%s"), *ActionAssetName, *ActionAssetName);

		UInputAction* FoundAction = LoadObject<UInputAction>(nullptr, *AssetPath);
		if (!FoundAction)
		{
			UE_LOG(LogTemp, Warning, TEXT("ApplyKeybindings: couldn't load InputAction %s (path: %s)"), *ActionAssetName, *AssetPath);
			continue;
		}

		// Recreate FKey from stored internal name
		FKey NewKey(*Binding->Keybind);
		if (!NewKey.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("ApplyKeybindings: invalid key string '%s' for action '%s'"), *Binding->Keybind, *Binding->Name);
			continue;
		}

		// Map key for the action on the mapping context
		// MapKey usually exists as a convenience wrapper exposed in the API you used earlier
		MappingContext->MapKey(FoundAction, NewKey);

		UE_LOG(LogTemp, Log, TEXT("ApplyKeybindings: mapped action %s -> key %s"), *Binding->Name, *NewKey.GetFName().ToString());
	}

	// Re-apply mapping context to the subsystem (remove old first if necessary)
	// You may want to remove then add to ensure updated mappings are used:
	Subsystem->RemoveMappingContext(MappingContext);
	Subsystem->AddMappingContext(MappingContext, 0);
}