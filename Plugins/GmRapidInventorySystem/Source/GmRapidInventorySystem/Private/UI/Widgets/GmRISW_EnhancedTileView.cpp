// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "UI/Widgets/GmRISW_EnhancedTileView.h"

#include "Inventory/GmRIS_InventoryItemInstance.h"
#include "UI/GmRISW_ItemTile.h"

UGmRISW_EnhancedTileView::UGmRISW_EnhancedTileView(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UGmRISW_EnhancedTileView::OnItemsChanged(const TArray<UObject*>& AddedItems, const TArray<UObject*>& RemovedItems)
{
	Super::OnItemsChanged(AddedItems, RemovedItems);
}

void UGmRISW_EnhancedTileView::OnItemClickedInternal(UObject* Item)
{
	if (!Item)
	{
		return;
	}
	
	if (const UGmRISW_ItemTile* CurrentClickedItemTile{Cast<UGmRISW_ItemTile>(GetEntryWidgetFromItem(Item))};
		CurrentClickedItemTile && Item->GetClass() != UGmRIS_InventoryItemInstance::StaticClass())
	{
		if (!CurrentClickedItemTile->GmIsReady())
		{
			return;
		}
		
		//@TODO Should I add it to debug mode?
		// if (const UGmRIS_InventoryItemInstance* SelectedItem{Cast<UGmRIS_InventoryItemInstance>(Item)})
		// {
			// UE_LOG(LogTemp, Error, L"Selected Item is %s.", *SelectedItem->FindFragmentByClass<UGmRIS_ItemFragment_InventoryTile>()->DisplayNameWhenAcquired.ToString());
		// }
		
		Super::OnItemClickedInternal(Item);
		
		// OnClickedAnyItem.Execute(Cast<UGmRIS_InventoryItemInstance>(Item));
	}
}

void UGmRISW_EnhancedTileView::SwapListItemsByIndex(const int64 InTargetA, const int64 InTargetB)
{
	if (InTargetA < 0 || InTargetB < 0 || InTargetA == InTargetB)
	{
		// UE_LOG(LogTemp, Error, L"SwapListItemsByIndex() : InTargetA and InTargetB are same or index less than 0.");
		return;
	}
	ListItems.Swap(InTargetA, InTargetB);
	RequestRefresh();
}

void UGmRISW_EnhancedTileView::SwapListItemsByPtr(UObject* InTargetA, UObject* InTargetB)
{
	if (!ListItems.Contains(InTargetA) || !ListItems.Contains(InTargetB))
	{
		// UE_LOG(LogTemp, Error, L"SwapListItemsByPtr() : InTargetA or InTargetB Can not found.");
		return;
	}
	SwapListItemsByIndex(ListItems.IndexOfByKey(InTargetA), ListItems.IndexOfByKey(InTargetB));
}

void UGmRISW_EnhancedTileView::SwapListItemsByEntryWidget(const IUserObjectListEntry* InEntryWidgetA,
	const IUserObjectListEntry* InEntryWidgetB)
{
	if (!InEntryWidgetA || !InEntryWidgetB)
	{
		// UE_LOG(LogTemp, Error, L"SwapListItemsByEntryWidget() : InTargetA or InTargetB is not valid.");
		return;
	}
	SwapListItemsByPtr(InEntryWidgetA->GetListItem<UObject>(), InEntryWidgetB->GetListItem<UObject>());
}
