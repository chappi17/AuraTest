// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Interface/GmRISI_Pickable.h"

#include <AbilitySystemComponent.h>
#include <GameplayCueManager.h>
#include <GameFramework/Actor.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerState.h>
#include <UObject/ScriptInterface.h>

#include "Core/GmGameplayTags.h"
#include "Equipment/GmRIS_QuickBarComponent.h"
#include "Inventory/GmRIS_InventoryManagerComponent.h"
#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_SetStats.h"

class UActorComponent;

UGmRISI_PickableStatics::UGmRISI_PickableStatics()
	:Super(FObjectInitializer::Get())
{}

TScriptInterface<IGmRISI_Pickable> UGmRISI_PickableStatics::GetFirstPickupAbleFromActor(AActor* InActor)
{
	// If the actor is directly pickupAble, return that.
	if (const TScriptInterface<IGmRISI_Pickable> PickupAbleActor(InActor); PickupAbleActor)
	{
		return PickupAbleActor;
	}

	// If the actor isn't pickupAble, it might have a component that has a pickupAble interface.
	if (TArray PickupAbleComponents{InActor ? InActor->GetComponentsByInterface(
		UGmRISI_Pickable::StaticClass()) : TArray<UActorComponent*>()}; PickupAbleComponents.Num() > 0)
	{
		// Get first pickupAble, if the user needs more sophisticated pickup distinction, will need to be solved elsewhere.
		return TScriptInterface<IGmRISI_Pickable>(PickupAbleComponents[0]);
	}

	return TScriptInterface<IGmRISI_Pickable>();
}

void UGmRISI_PickableStatics::AddPickupToInventory(UGmRIS_InventoryManagerComponent* InInventoryComponent,
   const TScriptInterface<IGmRISI_Pickable> InPickup)
{
	if (InInventoryComponent && InPickup)
	{
		APawn* ReceivingPawn{Cast<AController>(InInventoryComponent->GetOwner())->GetPawn()};
		UGmRIS_QuickBarComponent* QuickBarComponent{
			InInventoryComponent->GetOwner()->FindComponentByClass<UGmRIS_QuickBarComponent>()};
		UAbilitySystemComponent* CurrentAsc{ReceivingPawn->GetPlayerState()->FindComponentByClass<UAbilitySystemComponent>()};
		
		const auto& [Instances, Templates]
		{InPickup->GetPickupInventory()};
      
		for (const auto& [ItemDef/*, InteractionAbilityToGrant*/] : Templates)
		{
			// QuickBarComponent
			if (QuickBarComponent && ItemDef)
			{
				if (const int32 NextFreeItemSlot{QuickBarComponent->GetNextFreeItemSlot()};
					NextFreeItemSlot > -1 && ReceivingPawn)
				{
					FGameplayCueParameters NewGCParams;
					NewGCParams.Instigator = Cast<AActor>(InPickup.GetObject());
					if (CurrentAsc)
					{
						if (ReceivingPawn->GetLocalRole() == ROLE_Authority) // HasAuthority()
						{
							CurrentAsc->ExecuteGameplayCue(
								GmGameplayTags::TAG_GmRIS_GameplayCue_Interact_CanBeRegisteredQuickSlotPickUp,
								NewGCParams);
						}
					}
					else
					{
						UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
							ReceivingPawn,
							GmGameplayTags::TAG_GmRIS_GameplayCue_Interact_CanBeRegisteredQuickSlotPickUp,
							NewGCParams);
					}

					if (UGmRIS_InventoryItemInstance* Loc_ExistInventoryItemInst{
						InInventoryComponent->FindFirstItemStackByDefinition(ItemDef)})
					{
						if (const UGmRIS_ItemFragment_SetStats* StatFragment{
							Loc_ExistInventoryItemInst->FindFragmentByClass<UGmRIS_ItemFragment_SetStats>()})
						{
							Loc_ExistInventoryItemInst->AddStatTagStack(
								GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity,
								StatFragment->GetItemStatByTag(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
						}
					}
					else
					{
						QuickBarComponent->AddItemToSlot(NextFreeItemSlot,
							InInventoryComponent->AddItemDefinition(ItemDef));
					}
				}
			}//~End of Quick bar component.
			else
			{
				//~If the item already exists, add it to the stack.
				if (UGmRIS_InventoryItemInstance* Loc_ExistInventoryItemInst{
					InInventoryComponent->FindFirstItemStackByDefinition(ItemDef)})
				{
					if (const UGmRIS_ItemFragment_SetStats* StatFragment{
						Loc_ExistInventoryItemInst->FindFragmentByClass<UGmRIS_ItemFragment_SetStats>()})
					{
						Loc_ExistInventoryItemInst->AddStatTagStack(
							GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity,
							StatFragment->GetItemStatByTag(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
					}
					continue;
				}
				//~Add new item.(When you do not have "UGmRIS_ItemFragment_SetStats".)
				UE_LOG(LogTemp, Error,
					L"The Item Definition just added does not have a 'SetStat' Fragment. please check.");
				InInventoryComponent->AddItemDefinition(ItemDef, 1);
			}
		}//~End of template.

		

		// Add item from instances.
		for (const auto& [Item] : Instances)
		{
			// InInventoryComponent->AddItemInstance(Item);
			// QuickBarComponent
			
			if (const TSubclassOf ItemDef{Item->GetItemDefinition()})
			{
				if (const int32 NextFreeItemSlot{QuickBarComponent->GetNextFreeItemSlot()};
					NextFreeItemSlot > -1 && ReceivingPawn)
				{
					FGameplayCueParameters NewGCParams;
					NewGCParams.Instigator = Cast<AActor>(InPickup.GetObject());
					
					if (CurrentAsc)
					{
						if (ReceivingPawn->GetLocalRole() == ROLE_Authority) // HasAuthority()
						{
							CurrentAsc->ExecuteGameplayCue(
								GmGameplayTags::TAG_GmRIS_GameplayCue_Interact_CanBeRegisteredQuickSlotPickUp,
								NewGCParams);
						}
					}
					else
					{
						UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
							ReceivingPawn,
							GmGameplayTags::TAG_GmRIS_GameplayCue_Interact_CanBeRegisteredQuickSlotPickUp,
							NewGCParams);
					}
			
					if (UGmRIS_InventoryItemInstance* Loc_ExistInventoryItemInst{
						InInventoryComponent->FindFirstItemStackByDefinition(ItemDef)})
					{
						Loc_ExistInventoryItemInst->AddStatTagStack(
							GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity,
							Item->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
					}
					else
					{
						InInventoryComponent->AddItemInstance(Item);
						QuickBarComponent->AddItemToSlot(NextFreeItemSlot, Item);
					}
				}
			}//~End of Quick bar component
			else
			{
				//~If the item already exists, add it to the stack.
				if (UGmRIS_InventoryItemInstance* Loc_ExistInventoryItemInst{
					InInventoryComponent->FindFirstItemStackByDefinition(ItemDef)})
				{
					Loc_ExistInventoryItemInst->AddStatTagStack(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity,
						Item->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
					
					continue;
				}
				//~Add new item.(When you do not have "UGmRIS_ItemFragment_SetStats".)
				UE_LOG(LogTemp, Error,
					L"The Item Definition just added does not have a 'SetStat' Fragment. please check.");
				InInventoryComponent->AddItemInstance(Item);
			}
		}
	}
}
