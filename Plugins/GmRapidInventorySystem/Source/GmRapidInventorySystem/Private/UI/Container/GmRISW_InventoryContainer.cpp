// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "UI/Container/GmRISW_InventoryContainer.h"

#include <CommonLazyImage.h>
#include <CommonRichTextBlock.h>
#include <Animation/WidgetAnimation.h>
#include <Components/Button.h>
#include <Runtime/Launch/Resources/Version.h>

#include "Core/GmGameplayTags.h"
#include "Inventory/GmRIS_InventoryItemInstance.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_Consumable.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_InventoryTile.h"
#include "UI/GmRISW_InventoryPanel.h"
#include "UI/GmRISW_ItemTile.h"
#include "UI/Widgets/GmRISW_EnhancedTileView.h"

void UGmRISW_InventoryContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UGmRISW_InventoryContainer::NativeConstruct()
{
	GmSimpleInventoryGrid->InventoryGridQuantity = MainInventoryInitSettings.InventoryGridQuantity;
	
	Super::NativeConstruct();

	UGameplayMsgSubsystem::Get(this).RegisterListener(
		GmGameplayTags::TAG_GmRIS_QuickBar_Msg_SlotItemSynchronization, this, &ThisClass::UpdateMainInventory);

	// Init static Widgets setting.
	UsingItemBtn->SetIsEnabled(false);
	DropItemBtn->SetIsEnabled(false);
	
#if ENGINE_MAJOR_VERSION == 5
#if ENGINE_MINOR_VERSION <= 1
	GmSetItemDescImgOpacity(static_cast<bool>(ItemDescriptionImage->Brush.GetResourceObject()));
#else
	GmSetItemDescImgOpacity(static_cast<bool>(ItemDescriptionImage->GetBrush().GetResourceObject()));
#endif
#endif
	ItemLongDescriptionText->InlineIconDisplayMode = ERichTextInlineIconDisplayMode::TextOnly;
	ItemLongDescriptionText->SetAutoWrapText(true);

	// Bind static logic.
	GmSimpleInventoryGrid->InventoryTileView->OnItemSelectionChanged().Clear();
	GmSimpleInventoryGrid->InventoryTileView->OnItemSelectionChanged().AddLambda([this](UObject* InSelectedItemInst)->void
	{
		if (const UGmRISW_ItemTile* NewChangedItemTile{
			Cast<UGmRISW_ItemTile>(GmSimpleInventoryGrid->InventoryTileView->GetEntryWidgetFromItem(InSelectedItemInst))})
		{
			UE_CLOG(bIsDebug, LogTemp, Error, L"Current selected item index is %i.",
				GmSimpleInventoryGrid->InventoryTileView->GetIndexForItem(InSelectedItemInst));
			
			if (NewChangedItemTile->GmIsReady())
			{
				PlayAnimation(ItemInfoAppearanceAnim, 0.f, 1, EUMGSequencePlayMode::Forward,
					ItemDescriptionSettings.AppearanceAnimPlaybackMultiplier);

				const UGmRIS_ItemFragment_InventoryTile* CurrentInventoryItemFragment{NewChangedItemTile->GmGetInventoryTileFragment()};
					
				ItemDescriptionImage->SetBrushResourceObject(CurrentInventoryItemFragment->ItemViewWithinInfoPanel.GetResourceObject());
				ItemLongDescriptionText->SetText(CurrentInventoryItemFragment->ItemLongDescriptionText);

#if ENGINE_MAJOR_VERSION == 5
#if ENGINE_MINOR_VERSION <= 1
	GmSetItemDescImgOpacity(static_cast<bool>(ItemDescriptionImage->Brush.GetResourceObject()));
#else
	GmSetItemDescImgOpacity(static_cast<bool>(ItemDescriptionImage->GetBrush().GetResourceObject()));
#endif
#endif
				RefreshFirstItemExistsOrNot();
				return;
			}
		}
		
		ItemDescriptionImage->SetBrushResourceObject(nullptr);
		ItemDescriptionImage->SetRenderOpacity(0);
		ItemLongDescriptionText->SetText(FText::FromString(L"No items selected"));
		RefreshFirstItemExistsOrNot();
	});
	
	UsingItemBtn->OnClicked.AddUniqueDynamic(this, &ThisClass::UseAnItem);
	DropItemBtn->OnClicked.AddUniqueDynamic(this, &ThisClass::DropsAnItem);
	PlayAnimation(InventoryAppearance);

	RefreshFirstItemExistsOrNot();
}

void UGmRISW_InventoryContainer::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Init static widgets setting  from editor.
#if ENGINE_MAJOR_VERSION == 5
#if ENGINE_MINOR_VERSION <= 1
	GmSetItemDescImgOpacity(static_cast<bool>(ItemDescriptionImage->Brush.GetResourceObject()));
#else
	GmSetItemDescImgOpacity(static_cast<bool>(ItemDescriptionImage->GetBrush().GetResourceObject()));
#endif
#endif
	ItemLongDescriptionText->InlineIconDisplayMode = ERichTextInlineIconDisplayMode::TextOnly;
	ItemLongDescriptionText->SetAutoWrapText(true);
}

void UGmRISW_InventoryContainer::NativeDestruct()
{
	UsingItemBtn->OnClicked.Clear();
	DropItemBtn->OnClicked.Clear();
	
	AnimationCallbacks.RemoveAll([this](FAnimationEventBinding& InB)->bool
	{
		return InB.Animation == ItemInfoAppearanceAnim && InB.AnimationEvent == EWidgetAnimationEvent::Finished;
	});

	PlayAnimationReverse(InventoryAppearance);

	GmSimpleInventoryGrid->InventoryTileView->OnItemSelectionChanged().Clear();
	
	Super::NativeDestruct();
}

void UGmRISW_InventoryContainer::UpdateMainInventory(FGameplayTag,
	const FGmSimpleItemInstance& InTargetItemInst)
{
	// UE_CLOG(bIsDebug && !InTargetItemInst.ItemInst, LogTemp, Error,
		// L"MainInventory : UpdateMainInventory() - ItemInst is nullptr.");
	
	if (UGmRISW_ItemTile* ItemWidgetToRemove{Cast<UGmRISW_ItemTile>(
	GmSimpleInventoryGrid->InventoryTileView->GetEntryWidgetFromItem(
		GmSimpleInventoryGrid->InventoryTileView->GetSelectedItem<UGmRIS_InventoryItemInstance>()))})
	{
		// Checks whether the current quantity of an item is 0 or bIsDrop
		if ((InTargetItemInst.ItemInst &&
			InTargetItemInst.ItemInst->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity) < 1)
			|| InTargetItemInst.bIsDrop)
		{
			ItemWidgetToRemove->GmSetItemInst(nullptr);
			GmSimpleInventoryGrid->InventoryTileView->ClearSelection();
		}
		
		ItemWidgetToRemove->GmRefreshItemTileInfo();
	}
	
	// Refresh item description
	RefreshFirstItemExistsOrNot();
}

void UGmRISW_InventoryContainer::UseAnItem()
{
	if (UGmRIS_InventoryItemInstance* Loc_SelectedItem{
		GmSimpleInventoryGrid->InventoryTileView->GetSelectedItem<UGmRIS_InventoryItemInstance>()})
	{
		UE_CLOG(bIsDebug, LogTemp, Error, L"Current Used item index is %i.",
			GmSimpleInventoryGrid->InventoryTileView->GetIndexForItem(Loc_SelectedItem));
		
		UGameplayMsgSubsystem::Get(this).BroadcastMessage(
			GmGameplayTags::TAG_GmRIS_Inventory_Item_Using,
			FGmSimpleItemInstance(Loc_SelectedItem, GetOwningPlayer()));
	}
}

void UGmRISW_InventoryContainer::DropsAnItem()
{
	if (UGmRIS_InventoryItemInstance* Loc_SelectedItem{
		GmSimpleInventoryGrid->InventoryTileView->GetSelectedItem<UGmRIS_InventoryItemInstance>()})
	{
		// Using All Item
		UGameplayMsgSubsystem::Get(this).BroadcastMessage(
			GmGameplayTags::TAG_GmRIS_Inventory_Item_Using, FGmSimpleItemInstance(Loc_SelectedItem,
				GetOwningPlayer(), true, true));
	}
}

void UGmRISW_InventoryContainer::RefreshFirstItemExistsOrNot() const
{
	if (const UGmRISW_ItemTile* CurrentSelectedWidget{
		Cast<UGmRISW_ItemTile>(GmSimpleInventoryGrid->InventoryTileView->GetEntryWidgetFromItem(
			GmSimpleInventoryGrid->InventoryTileView->GetSelectedItem<UGmRIS_InventoryItemInstance>()))})
	{
		UsingItemBtn->SetIsEnabled(CurrentSelectedWidget->GmIsReady());
		
		bool bCanConsume{false}, bCanDrop{false};
		
		if (const UGmRIS_ItemFragment_Consumable* ConsumableOpt{
			CurrentSelectedWidget->GetListItem<UGmRIS_InventoryItemInstance>()->FindFragmentByClass<UGmRIS_ItemFragment_Consumable>()})
		{
			bCanConsume = ConsumableOpt->bIsConsumable;
			bCanDrop = ConsumableOpt->bCanDrop;
		}
		
		UsingItemBtn->SetIsEnabled(CurrentSelectedWidget->GmIsReady() && bCanConsume);
		DropItemBtn->SetIsEnabled(CurrentSelectedWidget->GmIsReady() && bCanDrop);
		return;
	}
	UsingItemBtn->SetIsEnabled(false);
	DropItemBtn->SetIsEnabled(false);
}

void UGmRISW_InventoryContainer::GmSetItemDescImgOpacity(const bool InIsOn) const
{
	ItemDescriptionImage->SetRenderOpacity(/*static_cast<float>(*/InIsOn/*)*/);
}
