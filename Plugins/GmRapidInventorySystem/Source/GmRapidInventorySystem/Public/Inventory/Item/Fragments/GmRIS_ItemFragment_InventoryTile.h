// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <Internationalization/Text.h>
#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include <Styling/SlateBrush.h>

#include "GmRIS_ItemFragment_InventoryTile.generated.h"

class USoundBase;
class UObject;

USTRUCT(BlueprintType)
struct FGmItemSoundOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	TObjectPtr<USoundBase> SoundWhenUsingAnItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	FVector2D UsingItemSoundVolumeAndPitch{FVector2D(1.0f, 1.0f)};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	TObjectPtr<USoundBase> SoundWhenDroppingAnItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	FVector2D DroppingItemSoundVolumeAndPitch{FVector2D(1.0f, 1.0f)};
	
};

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_ItemFragment_InventoryTile : public UGmRIS_InventoryItemFragment
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display)
	FSlateBrush TileViewWithinInventory;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display)
	FText DisplayNameWhenAcquired;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display, meta = (MultiLine = true))
	FText ItemShortToolTipText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display)
	FSlateBrush ItemViewWithinInfoPanel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Display, meta = (MultiLine = true))
	FText ItemLongDescriptionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	FGmItemSoundOptions ItemSoundOptions;
	
};
