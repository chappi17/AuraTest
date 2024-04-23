// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "UI/GmRISW_ItemTile.h"

#include <CommonBorder.h>
#include <CommonLazyImage.h>
#include <CommonTextBlock.h>
#include <Engine/Texture2D.h>

#include "Core/GmGameplayTags.h"
#include "Inventory/GmRIS_InventoryItemInstance.h"
#include "Inventory/GmRIS_InventoryManagerComponent.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_InventoryTile.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_SetStats.h"
#include "UI/Misc/GmRIS_DragDropOperation.h"
#include "UI/Widgets/GmRISW_EnhancedTileView.h"

void UGmRISW_ItemTile::NativeOnListItemObjectSet(UObject* ListItemObject)
{
   GmSetItemInst(Cast<UGmRIS_InventoryItemInstance>(ListItemObject));

   GmRefreshItemTileInfo();
}

void UGmRISW_ItemTile::GmOnSelectedUnitMainBorder(const bool InIsSelected) const
{
   const bool bCanClickable{InIsSelected && GmIsReady()};
   
   FSlateBrush NewBrush;
   
   NewBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
   NewBrush.TintColor = bCanClickable ?
      TileViewSelectedSettings.ItemBGColorWhenSelected : TileViewSelectedSettings.DefaultItemBGColor;
   NewBrush.OutlineSettings = FSlateBrushOutlineSettings(TileViewSelectedSettings.SelectedOutlineCornerRadius,
      TileViewSelectedSettings.SelectedOutlineColor, bCanClickable ?
      TileViewSelectedSettings.SelectedOutlineWidth : 0.f);
   
   UnitMainBorder->SetBrush(NewBrush);
}

void UGmRISW_ItemTile::GmSetItemTileSettings(const FGmTileViewSettings InNewItemTileSettings)
{
   TileViewSelectedSettings = InNewItemTileSettings;
}

void UGmRISW_ItemTile::NativeConstruct()
{
   Super::NativeConstruct();

   UGameplayMsgSubsystem::Get(this).RegisterListener(
      GmGameplayTags::TAG_GmRIS_QuickBar_Msg_SlotItemSynchronization, this, &ThisClass::SynchronizeSelectedItem);

   this->SetVisibility(ESlateVisibility::Visible);

   GmRefreshItemTileInfo();
}

void UGmRISW_ItemTile::NativeDestruct()
{
   Super::NativeDestruct();
}

void UGmRISW_ItemTile::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
   UDragDropOperation*& OutOperation)
{
   // InMouseEvent.IsMouseButtonDown()
   if (!GmIsReady())
   {
      return;
   }
   
   OutOperation = NewObject<UGmRIS_DragDropOperation>();
   OutOperation->DefaultDragVisual = this;
   OutOperation->Payload = this;
   OutOperation->Pivot = EDragPivot::MouseDown;
   
   Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UGmRISW_ItemTile::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
   UDragDropOperation* InOperation)
{
   if (!InOperation)
   {
      return false;
   }

   UGmRISW_ItemTile* InTarget{InOperation->Payload};

   if (!InTarget || InTarget == this)
   {
      return false;
   }
   if (UGmRISW_EnhancedTileView* CurrentTileView{Cast<UGmRISW_EnhancedTileView>(GetOwningListView())})
   {
      CurrentTileView->SwapListItemsByEntryWidget(InTarget, this);
   }
   
   if (UGmRIS_InventoryManagerComponent* CurrentInventoryComp{
      GetOwningPlayer()->FindComponentByClass<UGmRIS_InventoryManagerComponent>()})
   {
      CurrentInventoryComp->SwapItem(InTarget->ItemInst, ItemInst);
   }
   
   InTarget->GmRefreshItemTileInfo();
   
   GmRefreshItemTileInfo();
   
   return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

#if WITH_EDITOR
void UGmRISW_ItemTile::NativePreConstruct()
{
   Super::NativePreConstruct();

   GmRefreshItemTileInfo();
}
#endif

void UGmRISW_ItemTile::SynchronizeSelectedItem(FGameplayTag,
   const FGmSimpleItemInstance& InChangedInfo)
{
   if (InChangedInfo.ItemInst)
   {
      if (UGmRIS_InventoryItemInstance* ThisItemInst{
         GetListItem<UGmRIS_InventoryItemInstance>()}; InChangedInfo.ItemInst == ThisItemInst)
      {
         if (ThisItemInst->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity) < 1)
         {
            GmSetItemInst(nullptr);
         }
         
         GmRefreshItemTileInfo();
            
         GetOwningListView()->RequestRefresh();
      }
   }
}

void UGmRISW_ItemTile::GmRefreshItemTileInfo()
{
   if (GmIsReady())
   {
      const int32 CurrentAmount{ItemInst->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity)};
      if (CurrentAmount < 1)
      {
         GmSetItemInst(nullptr);
         GmRefreshItemTileInfo();
         return;
      }
      
      ItemLazyImage->SetBrushResourceObject(ItemInventoryTileFragment->TileViewWithinInventory.GetResourceObject());
      ItemSimpleName->SetText(ItemInventoryTileFragment->DisplayNameWhenAcquired);
      ItemQuantity->SetText(FText::FromString(FString::FromInt(CurrentAmount)));
      SetToolTipText(ItemInventoryTileFragment->ItemShortToolTipText);
      ItemLazyImage->SetOpacity(1);
      return;
   }

   ItemLazyImage->SetBrushResourceObject(nullptr);
   ItemSimpleName->SetText(FText::FromString(""));
   ItemQuantity->SetText(FText::FromString(""));
   GmOnSelectedUnitMainBorder(false);
   SetToolTipText(FText::FromString(""));
   ItemLazyImage->SetOpacity(0);
}

bool UGmRISW_ItemTile::GmSetItemInst(UGmRIS_InventoryItemInstance* InNewItemInst)
{
   ItemInst = InNewItemInst;
   if (!ItemInst)
   {
      return static_cast<bool>(ItemInventoryTileFragment = nullptr);
   }

   return static_cast<bool>(ItemInventoryTileFragment = Cast<UGmRIS_ItemFragment_InventoryTile>(
      ItemInst->FindFragmentByClass(UGmRIS_ItemFragment_InventoryTile::StaticClass())));
}

void UGmRISW_ItemTile::NativeOnItemSelectionChanged(const bool bIsSelected)
{
   GmOnSelectedUnitMainBorder(bIsSelected);
}
