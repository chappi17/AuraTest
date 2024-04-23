// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#include "Core/GmGameplayTags.h"

#include "GameplayTagsManager.h"

namespace GmGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_Inventory_Msg_StackChanged, L"GmRIS.Inventory.Msg.StackChanged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_Inventory_Item_Quantity, L"GmRIS.Inventory.Item.Quantity");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_Inventory_Item_Using, L"GmRIS.Inventory.Item.Using");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_Storage_Item_Using, L"GmRIS.Storage.Item.Using");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_Inventory_Msg_AddedItem, L"GmRIS.Inventory.Msg.AddedItem");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_Inventory_Msg_RemovedItem, L"GmRIS.Inventory.Msg.RemovedItem");

	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_GameplayCue_Interact_Pickup, L"GameplayCue.GmRapidInventorySystem.Interact.Collect");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_GameplayCue_Interact_CanBeRegisteredQuickSlotPickUp, L"GameplayCue.GmRapidInventorySystem.Interact.CahBeRegisteredQuickSlotPickUp");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_Inventory_ToggleInventory, L"GmRIS.Inventory.ToggleInventory");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interaction_Activate, L"Ability.Interaction.Activate");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interaction_Interact, L"Ability.Interaction.Interact");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_QuickBar_Msg_ActiveIndexChanged, L"GmRIS.QuickBar.Msg.ActiveIndexChanged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_QuickBar_Msg_SlotsChanged, L"GmRIS.QuickBar.Msg.SlotsChanged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GmRIS_QuickBar_Msg_SlotItemSynchronization, L"GmRIS.QuickBar.Msg.SlotItemSynchronization");

	// Input Tag
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Ability_QuickSlot_SelectSlot, L"InputTag.Ability.QuickSlot.SelectSlot");
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Ability_QuickSlot_CycleBackward, L"InputTag.Ability.QuickSlot.CycleBackward");
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Ability_QuickSlot_CycleForward, L"InputTag.Ability.QuickSlot.CycleForward");
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Ability_Quickslot_Drop, L"InputTag.Ability.QuickSlot.Drop");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Type_Action_Drop, L"Ability.Type.Action.Drop");


	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Type_Passive_ChangeQuickbarSlot, L"Ability.Type.Passive.ChangeQuickbarSlot");

	FGameplayTag FindTagByString(const FString& TagString, const bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager{UGameplayTagsManager::Get()};
		FGameplayTag Tag{Manager.RequestGameplayTag(FName(*TagString), false)};

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogTemp, Display, L"Could not find exact match for tag [%s] but found partial match on tag [%s].", *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}

