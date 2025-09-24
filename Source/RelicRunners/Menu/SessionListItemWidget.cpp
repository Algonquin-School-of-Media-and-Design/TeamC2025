
#include "SessionListItemWidget.h"
#include "SessionListItemData.h"

void USessionListItemWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void USessionListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    USessionListItemData* SessionData = Cast<USessionListItemData>(ListItemObject);
    if (SessionData && SessionNameText)
    {
        SessionNameText->SetText(FText::FromString(SessionData->SessionName));
        SessionNameText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)));
    }
}

void USessionListItemWidget::SetItemSelected(bool IsSelected)
{
    FLinearColor NewColor = IsSelected ? FLinearColor(1.0f, 1.0f, 0.0f, 1.0f) :
        FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);

    if (SessionNameText)
    {
        SessionNameText->SetColorAndOpacity(FSlateColor(NewColor));
    }
}