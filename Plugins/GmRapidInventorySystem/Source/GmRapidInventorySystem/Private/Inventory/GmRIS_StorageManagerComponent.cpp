// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Inventory/GmRIS_StorageManagerComponent.h"

#include "Core/GmGameplayTags.h"
#include "GameFramework/GameplayMsgSubsystem.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_SetStats.h"

UGmRIS_StorageManagerComponent::UGmRIS_StorageManagerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UGmRIS_StorageManagerComponent::BeginPlay() 
{
	Super::BeginPlay();

	UGameplayMsgSubsystem::Get(this).RegisterListener(
		GmGameplayTags::TAG_GmRIS_Storage_Item_Using, this, &ThisClass::UpdateSpecificStorageItems);

	AddInitialStorageItem(InitStorage);
}

void UGmRIS_StorageManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UGmRIS_StorageManagerComponent::AddInitialStorageItem(
	TArray<FGmUniqueStorageItemInfo> InInitialStorageItemDefs)
{
	if (InInitialStorageItemDefs.IsEmpty())
	{
		UE_CLOG(bIsDebug, LogTemp, Error, L"Add Initial Storage Item : In Initial Storage Item Defs Array is Empty");
		return;
	}
	for (const FGmUniqueStorageItemInfo Elem : InInitialStorageItemDefs)
	{
		if (!Elem.TargetItemDef)
		{
			continue;
		}
		AddItemDefinition(Elem.TargetItemDef);
	}
}

void UGmRIS_StorageManagerComponent::AddItemToStorage(const TSubclassOf<UGmRIS_InventoryItemDefinition> InTargetItemDef,
	const int32 InItemCount)
{
	if (InItemCount < 0 || !InTargetItemDef)
	{
		UE_CLOG(bIsDebug, LogTemp, Error, L"AddItemToStorage() : In Target Item Def is nullptr or In Item Count must be greater than 0.");
		return;
	}
	if (UGmRIS_InventoryItemInstance* FoundItemInst{FindFirstItemStackByDefinition(InTargetItemDef)})
	{
		FoundItemInst->AddStatTagStack(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity, InItemCount);
		for (const UGmRIS_InventoryItemInstance* Elem : GetAllItems())
		{
			if (Elem)
			{
				UE_CLOG(bIsDebug, LogTemp, Error, L"Item name is : %s, Count is %i.",
					*GetNameSafe(Elem->GetItemDefinition()), Elem->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
			}
		}
		return;
	}
	AddItemDefinition(InTargetItemDef, InItemCount);
	for (const UGmRIS_InventoryItemInstance* Elem : GetAllItems())
	{
		if (Elem)
		{
			UE_CLOG(bIsDebug, LogTemp, Error, L"Item name is : %s, Count is %i.",
				*GetNameSafe(Elem->GetItemDefinition()), Elem->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
		}
	}
}

void UGmRIS_StorageManagerComponent::RemoveItemToStorage(
	const TSubclassOf<UGmRIS_InventoryItemDefinition> InTargetItemDef, const bool InIsAll)
{
	if (!InTargetItemDef)
	{
		UE_CLOG(bIsDebug, LogTemp, Error, L"In Target Item Def is nullptr");
		return;
	}
	if (UGmRIS_InventoryItemInstance* FoundItemInst{FindFirstItemStackByDefinition(InTargetItemDef)})
	{
		if (InIsAll || FoundItemInst->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity) < 2)
		{
			RemoveItemInstance(FoundItemInst);
			return;
		}
		FoundItemInst->RemoveStatTagStack(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity, 1);
	}
}

void UGmRIS_StorageManagerComponent::UpdateSpecificStorageItems(FGameplayTag,
	const FGmUniqueStorageItemInfo& InNewItemInfo)
{
	// if (!InNewItemInfo.CanApplyInteraction(this->GetUniqueID()) || !InNewItemInfo.TargetItemDef)
	// {
	// 	return;
	// }
	//
	// UGmRIS_InventoryItemInstance* FoundItemInst{FindFirstItemStackByDefinition(InNewItemInfo.TargetItemDef)};
	//
	// if (InNewItemInfo.QuantityToAdd < 0)
	// {
	// 	UE_CLOG(bIsDebug, LogTemp, Error, L"")
	// }
	// else
	// {
	// 	
	// }
	//
	// for (const UGmRIS_InventoryItemInstance* Elem : GetAllItems())
	// {
	// 	if (Elem)
	// 	{
	// 		UE_CLOG(bIsDebug, LogTemp, Error, L"Item name is : %s, Count is %i.", *GetNameSafe(Elem->GetItemDefinition()), Elem->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
	// 	}
	// }
}
