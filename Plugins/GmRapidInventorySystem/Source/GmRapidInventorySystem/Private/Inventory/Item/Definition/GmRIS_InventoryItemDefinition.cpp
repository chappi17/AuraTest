// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"

#include "UObject/Class.h"

UGmRIS_InventoryItemDefinition::UGmRIS_InventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

const UGmRIS_InventoryItemFragment* UGmRIS_InventoryItemDefinition::FindFragmentByClass(
	const TSubclassOf<UGmRIS_InventoryItemFragment> InFragmentClassRef) const
{
	if (InFragmentClassRef)
	{
		for (const UGmRIS_InventoryItemFragment* Elem : Fragments)
		{
			if (!Elem || !Elem->IsA(InFragmentClassRef))
			{
				continue;
			}
			return Elem;
		}
	}
	return nullptr;
}

const UGmRIS_InventoryItemFragment* UGmRIS_InventoryFuncLib::FindItemDefinitionFragment(
	const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef, const TSubclassOf<UGmRIS_InventoryItemFragment> InFragmentClassRef)
{
	if (InItemDef && InFragmentClassRef)
	{
		return GetDefault<UGmRIS_InventoryItemDefinition>(InItemDef)->FindFragmentByClass(InFragmentClassRef);
	}
	return nullptr;
}
