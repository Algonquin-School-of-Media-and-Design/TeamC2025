#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 *
 */
UCLASS()
class RELICRUNNERS_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;
    class UModeSelectionWidget* ModeSelectionWidget;
    class UJoinUserWidget* JoinGameWidget;


public:
    UPROPERTY(meta = (BindWidget))
    class UWidgetSwitcher* WidgetSwitcher;

    UFUNCTION(BlueprintCallable)
    void ShowModeSelectionWidget();

    UFUNCTION(BlueprintCallable)
    void ShowJoinGameWidget();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> ModeSelectionUserWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> JoinGameUserWidgetClass;

};
