
#include "MainMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "JoinUserWidget.h"
#include "ModeSelectionWidget.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (WidgetSwitcher)
    {
        ModeSelectionWidget = CreateWidget<UModeSelectionWidget>(GetWorld(), ModeSelectionUserWidgetClass);
        JoinGameWidget = CreateWidget<UJoinUserWidget>(GetWorld(), JoinGameUserWidgetClass);

        if (ModeSelectionWidget && JoinGameWidget)
        {
            WidgetSwitcher->AddChild(ModeSelectionWidget);
            WidgetSwitcher->AddChild(JoinGameWidget);

            ModeSelectionWidget->SetParentMenu(this);
            JoinGameWidget->SetParentMenu(this);
            // Start by showing ModeSelectionWidget
            ShowModeSelectionWidget();
        }
    }
}

void UMainMenuWidget::ShowModeSelectionWidget()
{
    if (WidgetSwitcher && ModeSelectionWidget)
    {
        WidgetSwitcher->SetActiveWidget(ModeSelectionWidget);
    }
}

void UMainMenuWidget::ShowJoinGameWidget()
{
    if (WidgetSwitcher && JoinGameWidget)
    {
        WidgetSwitcher->SetActiveWidget(JoinGameWidget);
        JoinGameWidget->RefreshMenu();
    }
}
