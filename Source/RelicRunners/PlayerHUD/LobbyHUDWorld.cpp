// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyHUDWorld.h"
#include <RelicRunners/PlayerState/RelicRunnersPlayerState.h>
#include "Components/TextBlock.h"
#include "RelicRunners/PlayerPreview/LobbyPreview.h"

void ULobbyHUDWorld::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!OwningPreview || !OwningPreview->LinkedPlayerState) return;

    ARelicRunnersPlayerState* PS = OwningPreview->LinkedPlayerState;
    if (!PS) return;

    if (TB_Name)
        TB_Name->SetText(FText::FromString(PS->GetPlayerName()));

    if (TB_Class)
        TB_Class->SetText(FText::FromName(PS->SelectedClass));
}
