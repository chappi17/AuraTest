// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "UI/GmRISW_InventoryPanel.h"

// #include "Core/GmGameplayTags.h"
// #include "GameFramework/GameplayMsgSubsystem.h"
#include "Inventory/GmRIS_InventoryItemInstance.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_InventoryTile.h"
#include "UI/Widgets/GmRISW_EnhancedTileView.h"

void UGmRISW_InventoryPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGmRISW_InventoryPanel::NativeDestruct()
{
	Super::NativeDestruct();
}

void UGmRISW_InventoryPanel::ReGenerateItemTileView(TArray<UGmRIS_InventoryItemInstance*> InAllItems) const
{
	InventoryTileView->ClearListItems();
	InventoryTileView->RequestRefresh();
	
	for (int32 i{0}; i < InventoryGridQuantity; i++)
	{
		UGmRIS_InventoryItemInstance* NewItemElem{InAllItems.IsValidIndex(i) ? InAllItems[i] : NewObject<UGmRIS_InventoryItemInstance>()};
		InventoryTileView->AddItem(NewItemElem);
		
		if (UGmRISW_ItemTile* CurrentTileView{Cast<UGmRISW_ItemTile>(InventoryTileView->GetEntryWidgetFromItem(NewItemElem))})
		{
			CurrentTileView->GmSetItemTileSettings(ItemTileViewSettings);
			CurrentTileView->GmRefreshItemTileInfo();
		}
	}
}
