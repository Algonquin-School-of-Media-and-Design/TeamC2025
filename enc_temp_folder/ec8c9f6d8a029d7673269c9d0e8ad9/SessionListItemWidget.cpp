
#include "SessionListItemWidget.h"
#include "SessionListItemData.h"
#include "Components/Button.h"

void USessionListItemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SessionButton)
    {
        SessionButton->OnClicked.AddDynamic(this, &USessionListItemWidget::OnSessionButtonClicked);
    }
}

void USessionListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    USessionListItemData* SessionData = Cast<USessionListItemData>(ListItemObject);
    if (SessionData)
    {
        Setup(SessionData);
    }
}

void USessionListItemWidget::Setup(USessionListItemData* InData)
{
    StoredSessionData = InData;

    if (SessionNameText && StoredSessionData)
    {
        SessionNameText->SetText(FText::FromString(StoredSessionData->SessionName));
    }
}

void USessionListItemWidget::OnSessionButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Row Button Clicked: %s"),
        StoredSessionData ? *StoredSessionData->SessionName : TEXT("NoData"));

    if (StoredSessionData && OnSessionClicked.IsBound())
    {
        OnSessionClicked.Execute(StoredSessionData);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnSessionClicked not bound!"));
    }
}
