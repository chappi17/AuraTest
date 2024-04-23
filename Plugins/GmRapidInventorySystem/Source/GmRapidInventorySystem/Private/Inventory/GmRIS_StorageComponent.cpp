// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Inventory/GmRIS_StorageComponent.h"

#include <Components/AudioComponent.h>
#include "Core/GmGameplayTags.h"
#include <Engine/ActorChannel.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <Misc/AssertionMacros.h>
#include <Net/UnrealNetwork.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectPtr.h>
#include <UObject/UObjectBaseUtility.h>

#include "GameFramework/GameplayMsgSubsystem.h"
#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "Inventory/GmRIS_InventoryItemInstance.h"
#include "Global/GmRIS_GlobalFuncLib.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_NotificationInfo.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_SetStats.h"

class FLifetimeProperty;
struct FReplicationFlags;

FString FGmRISInventoryEntry::GetDebugString() const
{
	TSubclassOf<UGmRIS_InventoryItemDefinition> ItemDef;
	if (Instance)
	{
		ItemDef = Instance->GetItemDefinition();
	}

	return FString::Printf(L"%s (%d x %s)", *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

void FGmRISInventoryList::PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32)
{
	// UE_LOG(LogTemp, Error, L"PreReplicatedRemove");
	for (const int32 Index : InRemovedIndices)
	{
		FGmRISInventoryEntry& Stack{Entries[Index]};
		BroadcastChangeMsg(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FGmRISInventoryList::PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32)
{
	// UE_LOG(LogTemp, Error, L"PostReplicatedAdd");
	for (const int32 Index : InAddedIndices)
	{
		FGmRISInventoryEntry& Stack{Entries[Index]};
		BroadcastChangeMsg(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FGmRISInventoryList::PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32)
{
	// UE_LOG(LogTemp, Error, L"PostReplicatedChange");
	for (const int32 Index : InChangedIndices)
	{
		FGmRISInventoryEntry& Stack{Entries[Index]};
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMsg(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

UGmRIS_InventoryItemInstance* FGmRISInventoryList::AddEntry(
	const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemClass, const int32 InStackCount)
{
	check(InItemClass);
	check(OwnerComponent);

	const AActor* OwningActor{OwnerComponent->GetOwner()};
	check(OwningActor->HasAuthority());

	FGmRISInventoryEntry& NewEntry{Entries.AddDefaulted_GetRef()};
	
	NewEntry.Instance = NewObject<UGmRIS_InventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDefinition(InItemClass);
	for (const UGmRIS_InventoryItemFragment* Fragment : GetDefault<UGmRIS_InventoryItemDefinition>(InItemClass)->Fragments)
	{
		if (!Fragment)
		{
			continue;
		}
		Fragment->OnInstanceCreated(NewEntry.Instance);
	}
	NewEntry.StackCount = InStackCount;
	UGmRIS_InventoryItemInstance* Result{NewEntry.Instance};

	MarkItemDirty(NewEntry);

	return Result;
}

void FGmRISInventoryList::AddEntry(UGmRIS_InventoryItemInstance* InInstance)
{
	/*check(!"Unimplemented function called");*/ /*unimplemented();*/
	check(InInstance);
	check(OwnerComponent);
	
	const AActor* OwningActor{OwnerComponent->GetOwner()};
	check(OwningActor->HasAuthority());
	
	FGmRISInventoryEntry& NewEntry{Entries.AddDefaulted_GetRef()};
	
	const TSubclassOf/*<UGmRIS_InventoryItemDefinition>*/ CurrentDef{InInstance->GetItemDefinition()};
	
	NewEntry.Instance = InInstance;  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDefinition(CurrentDef);
	for (const UGmRIS_InventoryItemFragment* Fragment : GetDefault<UGmRIS_InventoryItemDefinition>(CurrentDef)->Fragments)
	{
		if (!Fragment || Fragment->GetClass() == UGmRIS_ItemFragment_SetStats::StaticClass())
		{
			continue;
		}
		Fragment->OnInstanceCreated(NewEntry.Instance);
	}
	NewEntry.StackCount = InInstance->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity);
	
	MarkItemDirty(NewEntry);
}

void FGmRISInventoryList::RemoveEntry(UGmRIS_InventoryItemInstance* InInstance)
{
	for (auto EntryIt{Entries.CreateIterator()}; EntryIt; ++EntryIt)
	{
		if (FGmRISInventoryEntry& Entry{*EntryIt}; Entry.Instance == InInstance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FGmRISInventoryList::BroadcastChangeMsg(const FGmRISInventoryEntry& OutEntry, const int32 InOldCount,
	const int32 InNewCount) const
{
	// UE_LOG(LogTemp, Error, L"Broadcast Change Inventory Item Stack.");
	UGameplayMsgSubsystem::Get(OwnerComponent->GetWorld()).BroadcastMessage(
		GmGameplayTags::TAG_GmRIS_Inventory_Msg_StackChanged,
		FGmRISInventoryChangeMsg(OwnerComponent,OutEntry.Instance, InNewCount, InNewCount - InOldCount,
			OwnerComponent ? OwnerComponent->GetUniqueID() : 0));
}

TArray<UGmRIS_InventoryItemInstance*> FGmRISInventoryList::GetAllItems() const
{
	TArray<UGmRIS_InventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());

	for (const FGmRISInventoryEntry& Entry : Entries)
	{
		if (!Entry.Instance) //@TODO: Should I not continue for an empty inventory slot? TnT.. - By DevGaeMyo
		{
			continue;
		}
		Results.Add(Entry.Instance);
	}

	return Results;
}

///////////////////////////////////////////////////////////////
/// UGmRIS_InventoryManagerComponent
/// 
UGmRIS_StorageComponent::UGmRIS_StorageComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer), Storage(this)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

}

bool UGmRIS_StorageComponent::CanAddItemDefinition(TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef,
	int32 InStackCount)
{
	// @TODO: Add support for stack limit / uniqueness checks / etc...
	// for (UGmRIS_InventoryItemInstance* ElemInventoryItemInst : InventoryList.GetAllItems())
	// {
	// 	if (ElemInventoryItemInst->GetItemDefinition()->GetClass() == InItemDef)
	// 	{
	// 		
	// 	}
	// }
	return true;
}

UGmRIS_InventoryItemInstance* UGmRIS_StorageComponent::AddItemDefinition(
	const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef, const int32 InStackCount,
	const bool InIsInitInventorySetting)
{
	UGmRIS_InventoryItemInstance* Result{nullptr};
	if (InItemDef)
	{
		Result = Storage.AddEntry(InItemDef, InStackCount);

		if (Result)
		{
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}

			// Broadcast notify added item
			if (const UGmRIS_ItemFragment_NotificationInfo* NotificationFragment{
				Result->FindFragmentByClass<UGmRIS_ItemFragment_NotificationInfo>()};
				NotificationFragment && !InIsInitInventorySetting)
			{
				if (NotificationFragment->NotificationBroadcastType == EGmNotificationType::EOnlyAdded ||
					NotificationFragment->NotificationBroadcastType == EGmNotificationType::BothAddedAndRemoved)
				{
#if !UE_BUILD_SHIPPING
					if (bIsDebug)
					{
						// Debug Screen.
						UGmRIS_GlobalFuncLib::DebugPrintControllerRoles(Cast<AController>(GetOuter()));
					}
#endif
					Client_NotifyAddedEvent(NotificationFragment, Result->GetStatTagStackCount(
						GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
				}
			}//~Broadcast notify added item
		}
	}
	return Result;
}

void UGmRIS_StorageComponent::AddItemInstance(UGmRIS_InventoryItemInstance* InItemInstance,
	const bool InIsInitInventorySetting)
{
	Storage.AddEntry(InItemInstance);
	
	if (InItemInstance)
	{
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(InItemInstance);
		}

		// Broadcast Notify added item.
		if (const UGmRIS_ItemFragment_NotificationInfo* NotificationFragment{
			InItemInstance->FindFragmentByClass<UGmRIS_ItemFragment_NotificationInfo>()};
			NotificationFragment && !InIsInitInventorySetting)
		{
			if (NotificationFragment->NotificationBroadcastType == EGmNotificationType::EOnlyAdded ||
				NotificationFragment->NotificationBroadcastType == EGmNotificationType::BothAddedAndRemoved)
			{
				Client_NotifyAddedEvent(NotificationFragment, InItemInstance->GetStatTagStackCount(
					GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
			}
		}
	}
}

void UGmRIS_StorageComponent::Client_NotifyAddedEvent_Implementation(
	const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InAddedCount)
{
	Internal_ClientNotifyAddedEvent(InNotifyFragmentInfo, InAddedCount);
}

void UGmRIS_StorageComponent::Client_NotifyRemovedEvent_Implementation(
	const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InAddedCount)
{
	Internal_ClientNotifyRemovedEvent(InNotifyFragmentInfo, InAddedCount);
}

void UGmRIS_StorageComponent::RemoveItemInstance(UGmRIS_InventoryItemInstance* InItemInstance)
{
	Storage.RemoveEntry(InItemInstance);
	
	if (InItemInstance)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(InItemInstance);
		}

		if (GetOwnerRole() == ROLE_Authority)
		{
			// Broadcast notify removed item
			if (const UGmRIS_ItemFragment_NotificationInfo* NotificationFragment{
				InItemInstance->FindFragmentByClass<UGmRIS_ItemFragment_NotificationInfo>()})
			{
				if (const int32 CurrentItemStackCount{InItemInstance->GetStatTagStackCount(
						GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity)};
						(NotificationFragment->NotificationBroadcastType == EGmNotificationType::EOnlyRemoved ||//@TODO Need?
							NotificationFragment->NotificationBroadcastType == EGmNotificationType::BothAddedAndRemoved) &&
							CurrentItemStackCount > 0)
				{
					Client_NotifyRemovedEvent(NotificationFragment, CurrentItemStackCount);
				}
			}//~Broadcast notify removed item
		}
	}
}

bool UGmRIS_StorageComponent::HasItemStackByItemInst(const UGmRIS_InventoryItemInstance* InItemInstToFind)
{
	return InItemInstToFind && GetAllItems().Contains(InItemInstToFind);
}

TArray<UGmRIS_InventoryItemInstance*> UGmRIS_StorageComponent::GetAllItems() const
{
	return Storage.GetAllItems();
}

UGmRIS_InventoryItemInstance* UGmRIS_StorageComponent::FindFirstItemStackByDefinition(
	const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef) const
{
	for (const FGmRISInventoryEntry& Entry : Storage.Entries)
	{
		if (UGmRIS_InventoryItemInstance* Instance{Entry.Instance}; IsValid(Instance))
		{
			if (Instance->GetItemDefinition() == InItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

bool UGmRIS_StorageComponent::SwapItem(UGmRIS_InventoryItemInstance* InTargetA, UGmRIS_InventoryItemInstance* InTargetB)
{
	if (!InTargetA || !InTargetB || InTargetA == InTargetB)
	{
		return false;
	}
	int32 IndexA{-1}, IndexB{-1};
	for (int32 i{0}; i < Storage.Entries.Num(); i++)
	{
		if (Storage.Entries[i].Instance == InTargetA)
		{
			IndexA = i;
		}
		if (Storage.Entries[i].Instance == InTargetB)
		{
			IndexB = i;
		}
	}
	if (IndexA < 0 || IndexB < 0)
	{
		return false;
	}
	Storage.Entries.Swap(IndexA, IndexB);
	return true;
}

int32 UGmRIS_StorageComponent::GetTotalItemCountByDefinition(
	const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef) const
{
	int32 TotalCount{0};
	for (const FGmRISInventoryEntry& Entry : Storage.Entries)
	{
		if (const UGmRIS_InventoryItemInstance* Instance{Entry.Instance}; IsValid(Instance))
		{
			if (Instance->GetItemDefinition() == InItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UGmRIS_StorageComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool WroteSomething{Super::ReplicateSubobjects(Channel, Bunch, RepFlags)};

	for (FGmRISInventoryEntry& Entry : Storage.Entries)
	{
		if (UGmRIS_InventoryItemInstance* Instance{Entry.Instance}; Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UGmRIS_StorageComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UGmRIS_InventoryItemInstance
	if (IsUsingRegisteredSubObjectList()/*Registered Sub Object List를 Using?*/)
	{
		for (const FGmRISInventoryEntry& Entry : Storage.Entries)
		{
			if (UGmRIS_InventoryItemInstance* Instance{Entry.Instance}; IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

void UGmRIS_StorageComponent::Internal_ClientNotifyAddedEvent(
	const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InAddedCount) const
{
}

void UGmRIS_StorageComponent::Internal_ClientNotifyRemovedEvent(
	const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InRemovedCount) const
{
}

void UGmRIS_StorageComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Storage);
}
