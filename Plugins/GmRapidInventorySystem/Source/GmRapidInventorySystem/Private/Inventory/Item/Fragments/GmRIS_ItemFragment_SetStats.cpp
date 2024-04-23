// Copyright 2024 Dev.GaeMyo. All Rights Reserved.


#include "Inventory/Item/Fragments/GmRIS_ItemFragment_SetStats.h"

#include "Inventory/GmRIS_InventoryItemInstance.h"

void UGmRIS_ItemFragment_SetStats::OnInstanceCreated(UGmRIS_InventoryItemInstance* InInst) const
{
	for (const auto& InitialItemStat : InitialItemStats)
	{
		InInst->AddStatTagStack(InitialItemStat.Key, InitialItemStat.Value, true);
	}
}

int32 UGmRIS_ItemFragment_SetStats::GetItemStatByTag(FGameplayTag InTag) const
{
	if (const int32* StatPtr{InitialItemStats.Find(InTag)})
	{
		return *StatPtr;
	}
	return 0;
}

int32 UGmRIS_ItemFragment_SetStats::GetNumberConsumedPerTime() const
{
	return NumberConsumedPerTime;
}
