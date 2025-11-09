#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Keybinds.generated.h"

USTRUCT(BlueprintType)
struct FKeybinding
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keybinding")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keybinding")
    FKey Bind;
};

UCLASS(Blueprintable, BlueprintType)
class RELICRUNNERS_API UKeybinds : public UObject
{
    GENERATED_BODY()

public:
    UKeybinds();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TArray<FKeybinding> KeyBinds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool InvertedXMouse = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool InvertedYMouse = false;
};
