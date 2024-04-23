// Copyright 2024 Dev.GaeMyo. All Rights Reserved.


#include "UI/Widgets/GmCW_QuickBarSlot.h"

#include <CommonNumericTextBlock.h>
#include <Components/Image.h>
#include <Materials/MaterialInstanceDynamic.h>

#include "Core/GmGameplayTags.h"
#include "Equipment/GmRIS_QuickBarComponent.h"
#include "GameFramework/GameplayMsgSubsystem.h"
#include "Inventory/GmRIS_InventoryItemInstance.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_QuickSlotIcon.h"

void UGmCW_QuickBarSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UGmCW_QuickBarSlot::NativeConstruct()
{
	Super::NativeConstruct();

	GmSimpleUpdateSlot(nullptr);
	UGameplayMsgSubsystem& Loc_GameplayMsgSubSystem{UGameplayMsgSubsystem::Get(this)};
	ActiveIndexChangedHandle = Loc_GameplayMsgSubSystem.RegisterListener(
		GmGameplayTags::TAG_GmRIS_QuickBar_Msg_ActiveIndexChanged, this, &ThisClass::GmQuickBarActiveIndexChangedFunc);
	QuickBarSlotsChangedHandle = Loc_GameplayMsgSubSystem.RegisterListener(
		GmGameplayTags::TAG_GmRIS_QuickBar_Msg_SlotsChanged, this, &ThisClass::GmQuickBarSlotsChangedFunc);
}

void UGmCW_QuickBarSlot::NativeDestruct()
{
	UGameplayMsgSubsystem& Loc_GameplayMsgSubSystem{UGameplayMsgSubsystem::Get(this)};

	Loc_GameplayMsgSubSystem.UnregisterListener(ActiveIndexChangedHandle);
	Loc_GameplayMsgSubSystem.UnregisterListener(QuickBarSlotsChangedHandle);
	
	Super::NativeDestruct();
}

void UGmCW_QuickBarSlot::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (QuickBarComponentWPtr.IsValid())
	{
		UpdateItemQuantityLeft(QuickBarComponentWPtr.Get());
	}
	else
	{
		UpdateComponentRef(GetOwningPlayer()->FindComponentByClass<UGmRIS_QuickBarComponent>());
	}
}

void UGmCW_QuickBarSlot::GmSimpleUpdateSlot(const UGmRIS_InventoryItemInstance* InItemInst)
{
	if (InItemInst)
	{
		if (const UGmRIS_ItemFragment_QuickSlotIcon* Loc_QuickSlotDisplayInfo{
			Cast<UGmRIS_ItemFragment_QuickSlotIcon>(
				InItemInst->FindFragmentByClass(UGmRIS_ItemFragment_QuickSlotIcon::StaticClass()))})
		{
			ItemCard->GetDynamicMaterial()->SetTextureParameterValue(
				ItemCardTextureParamName, Loc_QuickSlotDisplayInfo->T_ItemCardIcon);
			
			SubMiniIcon->GetDynamicMaterial()->SetTextureParameterValue(
				ItemSubMiniIconTextureParamName, Loc_QuickSlotDisplayInfo->T_ItemCardSubMini);
			
			if (bIsEmpty)
			{
				bIsEmpty = false;
				PlayAnimationReverse(ToEmpty);
				
				if (IsThisTheActiveQuickBarSlot())
				{
					return;
				}
				
				PlayAnimationForward(ActiveToInactive);
				return;
			}
			return;
		}
	}
	if (!bIsEmpty)
	{
		bIsEmpty = true;
	}
	PlayAnimationForward(ToEmpty);
}

bool UGmCW_QuickBarSlot::IsThisTheActiveQuickBarSlot() const
{
	if (!QuickBarComponentWPtr.IsValid())
	{
		return false;
	}
	return QuickBarComponentWPtr->GetActiveSlotIndex() == SlotIndex;
}

void UGmCW_QuickBarSlot::UpdateIsSelected()
{
	if (!bIsEmpty)
	{
		if (IsThisTheActiveQuickBarSlot())
		{
			bIsSelected = true;
			PlayAnimationForward(InactiveToActive);
		}
		else if(bIsSelected)
		{
			if (IsAnimationPlaying(InactiveToActive))
			{
				StopAnimation(InactiveToActive);
			}
			bIsSelected = false;
			PlayAnimationForward(ActiveToInactive);
		}
	}
}

void UGmCW_QuickBarSlot::UpdateComponentRef(UGmRIS_QuickBarComponent* InQuickBarComponent)
{
	if(!InQuickBarComponent || QuickBarComponentWPtr == InQuickBarComponent) return;

	QuickBarComponentWPtr = InQuickBarComponent;
	
	GmSimpleUpdateSlot(
		QuickBarComponentWPtr->GetSlots().IsValidIndex(SlotIndex) ?
		QuickBarComponentWPtr->GetSlots()[SlotIndex] : nullptr);
	
	UpdateIsSelected();
}

void UGmCW_QuickBarSlot::UpdateItemQuantityLeft(const UGmRIS_QuickBarComponent* InQuickBarComponent) const
{
	if (InQuickBarComponent->GetSlots().IsValidIndex(SlotIndex) && !bIsEmpty)
	{
		ItemQuantity->SetCurrentValue(
			InQuickBarComponent->GetSlots()[SlotIndex]->GetStatTagStackCount(
				GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
	}
}

void UGmCW_QuickBarSlot::GmQuickBarActiveIndexChangedFunc(FGameplayTag,
	const FGmRISQuickBarActiveIndexChangedMsg& InPayload)
{
	if (GetOwningPlayer() == InPayload.Owner && InPayload.Owner)
	{
		UpdateIsSelected();
		// UE_LOG(LogTemp, Error, L"GmQuickBarActiveIndexChangedFunc() QuickBarSlot Owner is : %s.", *GetNameSafe(InPayload.Owner));
	}
}

void UGmCW_QuickBarSlot::GmQuickBarSlotsChangedFunc(FGameplayTag,
	const FGmRISQuickBarSlotsChangedMsg& InPayload)
{
	if (InPayload.Owner && GetOwningPlayer()/* ? GetOwningPlayer() : UGmRIS_GlobalFuncLib::GetPCFromSpecificIndex(this, 0))*/
		== InPayload.Owner && InPayload.Slots.IsValidIndex(SlotIndex))
	{
		GmSimpleUpdateSlot(InPayload.Slots[SlotIndex]);
		// UE_LOG(LogTemp, Error, L"GmQuickBarSlotsChangedFunc() GetOwningPlayerPawn is : %s.", *GetNameSafe(GetOwningPlayerPawn()));
	}
}
