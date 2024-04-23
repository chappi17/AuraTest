// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "GmRIS_ItemFragment_Equippable.generated.h"

class UGmRIS_EquipmentDefinition;
class UObject;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_ItemFragment_Equippable : public UGmRIS_InventoryItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = GmRIS)
	TSubclassOf<UGmRIS_EquipmentDefinition> EquipmentDefinition;
};
