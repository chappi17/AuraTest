// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "GmRIS_ItemFragment_SetStats.generated.h"

class UGmRIS_InventoryItemInstance;
class UObject;
struct FGameplayTag;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_ItemFragment_SetStats : public UGmRIS_InventoryItemFragment
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Item Init Setting")
	TMap<FGameplayTag, int32> InitialItemStats;

	UPROPERTY(EditDefaultsOnly, Category = "Item Init Setting")
	int32 NumberConsumedPerTime{1};

public:
	
	virtual void OnInstanceCreated(UGmRIS_InventoryItemInstance* InInst) const override;

	int32 GetItemStatByTag(FGameplayTag InTag) const;
	int32 GetNumberConsumedPerTime() const;
	
};
