// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "GmRIS_ItemFragment_QuickSlotIcon.generated.h"

class UTexture;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_ItemFragment_QuickSlotIcon : public UGmRIS_InventoryItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Appearance)
	TObjectPtr<UTexture> T_ItemCardIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Appearance)
	TObjectPtr<UTexture> T_ItemCardSubMini;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Appearance)
	FText DisplayNameInQuickSlot;
	
};
