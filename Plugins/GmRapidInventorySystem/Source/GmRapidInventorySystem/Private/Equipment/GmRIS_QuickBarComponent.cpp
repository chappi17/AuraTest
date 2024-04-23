// Copyright 2024 Dev.GaeMyo. All Rights Reserved.


#include "Equipment/GmRIS_QuickBarComponent.h"

#include <NativeGameplayTags.h>
#include <Components/AudioComponent.h>
#include "Core/GmGameplayTags.h"
#include "Equipment/GmRIS_EquipmentManagerComponent.h"
#include "Equipment/Item/Definition/GmRIS_EquipmentDefinition.h"
#include "Equipment/Item/Fragments/GmRIS_ItemFragment_Equippable.h"
#include "GameFramework/GameplayMsgSubsystem.h"
#include <GameFramework/Pawn.h>
#include "Equipment/GmRIS_EquipmentInstance.h"
#include "Inventory/GmRIS_InventoryItemInstance.h"
#include "Inventory/GmRIS_InventoryManagerComponent.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_Consumable.h"
#include <Net/UnrealNetwork.h>

#include "Abilities/GameplayAbilityTypes.h"

class FLifetimeProperty;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_GmRIS_QuickBar_Msg_SlotsChanged, "GmRIS.QuickBar.Msg.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_GmRIS_QuickBar_Msg_ActiveIndexChanged, "GmRIS.QuickBar.Msg.ActiveIndexChanged");

UGmRIS_QuickBarComponent::UGmRIS_QuickBarComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bEditableWhenInherited = true;
	
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UGmRIS_QuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots - 1)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}
	
	Super::BeginPlay();

	if (const APlayerController* CurPC{Cast<APlayerController>(GetOuter())}; CurPC && CurPC->IsLocalPlayerController())
	{
		UGameplayMsgSubsystem::Get(this).RegisterListener(
		GmGameplayTags::TAG_GmRIS_QuickBar_Msg_SlotItemSynchronization,
	this, &ThisClass::QuickSlotItemSynchronization);
	}
}

// void UGmRIS_QuickBarComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
	// FActorComponentTickFunction* ThisTickFunction)
// {
	// Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// @TODO Debug
	// CurrentDeltaTime += DeltaTime;
	// if (CurrentDeltaTime > 1.f)
	// {
		// UE_CLOG(bIsDebug, LogTemp, Error, L"Current Quick Slots are~");
		// for (int32 i = 0; i < GetSlots().Num(); i++)
		// {
			// if (GetSlots().IsValidIndex(i) && IsValid(GetSlots()[i]))
			// {
				// UE_CLOG(bIsDebug, LogTemp, Error, L"Index %i : Item is %s.", i, *GetNameSafe(GetSlots()[i]->GetItemDefinition()))
			// }
		// }
		
		// CurrentDeltaTime = 0;
	// }
// }

void UGmRIS_QuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex{(ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex)};
	int32 NewIndex{ActiveSlotIndex};
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex])
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UGmRIS_QuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex{(ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex)};
	int32 NewIndex{ActiveSlotIndex};
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex])
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UGmRIS_QuickBarComponent::QuickSlotItemSynchronization(FGameplayTag,
	const FGmSimpleItemInstance& InTargetItemInst)
{
	Server_QuickSlotItemSynchronization(InTargetItemInst);
	
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		Internal_QuickSlotItemSynchronization(InTargetItemInst);
	}
}

void UGmRIS_QuickBarComponent::Server_QuickSlotItemSynchronization_Implementation(
	const FGmSimpleItemInstance& InTargetItemInst)
{
	Internal_QuickSlotItemSynchronization(InTargetItemInst);
}

void UGmRIS_QuickBarComponent::Internal_QuickSlotItemSynchronization(const FGmSimpleItemInstance& InTargetItemInst)
{
	// UE_CLOG(bIsDebug, LogTemp, Error, L"QuickSlotItemSynchronization() : GetOuter is %s.", *GetNameSafe(GetOuter()));
	
	if (!InTargetItemInst.ItemInst || !GetSlots().Contains(InTargetItemInst.ItemInst))
	{
		return;
	}
	
	// QuickBarComponent also removes items with a quantity of 0.
	if (InTargetItemInst.bIsDrop || InTargetItemInst.ItemInst->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity) < 1)
	{
		// Check whether the current item quantity is 0 and whether the currently activated item matches the target item.
		RemoveItemFromSlot(GetSlots().Find(InTargetItemInst.ItemInst));
		// UE_CLOG(bIsDebug, LogTemp, Error, L"QuickSlotItemSynchronization() - RemoveItemFromSlot");
	}
}

void UGmRIS_QuickBarComponent::SetActiveSlotIndex_Implementation(const int32 InNewIndex)
{
	// Checked to be ignored on first attempt.
	if (!FirstSelectConsumableItem)
	{
		FirstSelectConsumableItem = true;
		return;
	}
	
	if (Slots.IsValidIndex(InNewIndex))
	{
		if (ActiveSlotIndex != InNewIndex)
		{
			UnEquipItemInSlot();
			
			ActiveSlotIndex = InNewIndex;
    
			EquipOrConsumeItemInSlot();
			OnRep_ActiveSlotIndex();
		}
		else
		{
			//@TODO Need?
			if (UGmRIS_InventoryItemInstance* TargetItemInst{GetActiveSlotItem()})
			{
				GmBroadcastUsingItem(TargetItemInst);
			}
		}
	}
}

void UGmRIS_QuickBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

UGmRIS_InventoryItemInstance* UGmRIS_QuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 UGmRIS_QuickBarComponent::GetNextFreeItemSlot() const
{
	int32 SlotIndex{0};
	for (TObjectPtr<UGmRIS_InventoryItemInstance> ItemPtr : Slots)
	{
		if (!ItemPtr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void UGmRIS_QuickBarComponent::AddItemToSlot(const int32 InSlotIndex, UGmRIS_InventoryItemInstance* InItem)
{
	if (Slots.IsValidIndex(InSlotIndex) && InItem)
	{
		if (!Slots[InSlotIndex])
		{
			Slots[InSlotIndex] = InItem;
			OnRep_Slots();
		}
	}
}

UGmRIS_InventoryItemInstance* UGmRIS_QuickBarComponent::RemoveItemFromSlot(const int32 InSlotIndex)
{
	// UE_CLOG(bIsDebug, LogTemp, Error, L"RemoveItemFromSlot() is run.");
	
	UGmRIS_InventoryItemInstance* Result{nullptr};

	if (ActiveSlotIndex == InSlotIndex)
	{
		UnEquipItemInSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(InSlotIndex))
	{
		Result = Slots[InSlotIndex];

		if (Result)
		{
			Slots[InSlotIndex] = nullptr;
			OnRep_Slots();
		}
	}

	return Result;
}

void UGmRIS_QuickBarComponent::OnRep_Slots()
{
	UGameplayMsgSubsystem::Get(this).BroadcastMessage(
		TAG_GmRIS_QuickBar_Msg_SlotsChanged,
		FGmRISQuickBarSlotsChangedMsg(GetOwner(), Slots));
}

void UGmRIS_QuickBarComponent::OnRep_ActiveSlotIndex()
{
	UGameplayMsgSubsystem::Get(this).BroadcastMessage(
		TAG_GmRIS_QuickBar_Msg_ActiveIndexChanged,
		FGmRISQuickBarActiveIndexChangedMsg(GetOwner(), ActiveSlotIndex));
}

UGmRIS_EquipmentManagerComponent* UGmRIS_QuickBarComponent::FindEquipmentManager() const
{
	if (const AController* OwnerC{Cast<AController>(GetOwner())})
	{
		if (const APawn* Loc_Pawn{OwnerC->GetPawn()})
		{
			return Loc_Pawn->FindComponentByClass<UGmRIS_EquipmentManagerComponent>();
		}
	}
	return nullptr;
}

void UGmRIS_QuickBarComponent::UnEquipItemInSlot()
{
	if (!Slots.IsValidIndex(ActiveSlotIndex))
	{
		return;
	}
	
	if (UGmRIS_EquipmentManagerComponent* Loc_EquipmentManager{FindEquipmentManager()};
		Loc_EquipmentManager && EquippedItem)
	{
		Loc_EquipmentManager->UnEquipItem(EquippedItem);
		EquippedItem = nullptr;
	}
}

void UGmRIS_QuickBarComponent::EquipOrConsumeItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(!EquippedItem);

	if (UGmRIS_InventoryItemInstance* SlotItem{Slots[ActiveSlotIndex]})
	{
		if (const UGmRIS_ItemFragment_Equippable* EquipInfo{SlotItem->FindFragmentByClass<UGmRIS_ItemFragment_Equippable>()})
		{
			if (const TSubclassOf EquipDef{EquipInfo->EquipmentDefinition}; EquipDef)
			{
				if (UGmRIS_EquipmentManagerComponent* EquipmentManager{FindEquipmentManager()})
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);
					
					if (EquippedItem)
					{
						EquippedItem->SetInstigator(SlotItem);
					}
				}
			}
		}
	}
}

void UGmRIS_QuickBarComponent::GmBroadcastUsingItem_Implementation(UGmRIS_InventoryItemInstance* InSlotItem) const
{
	if (InSlotItem)
	{
		UGameplayMsgSubsystem::Get(this).BroadcastMessage(
		GmGameplayTags::TAG_GmRIS_Inventory_Item_Using,
		FGmSimpleItemInstance(InSlotItem,GetOwner()));
	}
}
