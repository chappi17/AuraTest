// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Inventory/GmRIS_InventoryItemInstance.h"

#include <Containers/Array.h>
#include <GameplayTagContainer.h>
#include <GameFramework/Controller.h>
#include <Misc/AssertionMacros.h>
#include <Net/UnrealNetwork.h>
#include <UObject/Class.h>

#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "Global/GmRIS_GlobalFuncLib.h"
#include "Inventory/GmRIS_InventoryManagerComponent.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_NotificationInfo.h"

#if UE_WITH_IRIS
#include <Iris/ReplicationSystem/ReplicationFragmentUtil.h>
#endif // UE_WITH_IRIS

class FLifetimeProperty;

UGmRIS_InventoryItemInstance::UGmRIS_InventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UGmRIS_InventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

#if UE_WITH_IRIS
void UGmRIS_InventoryItemInstance::RegisterReplicationFragments(
	UE::Net::FFragmentRegistrationContext& OutContext, UE::Net::EFragmentRegistrationFlags InRegistrationFlags)
{
	using namespace UE::Net;

	Super::RegisterReplicationFragments(OutContext, InRegistrationFlags);

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, OutContext, InRegistrationFlags);
}
#endif // UE_WITH_IRIS

void UGmRIS_InventoryItemInstance::AddStatTagStack(const FGameplayTag InTag, const int32 InStackCount,
	const bool InIsInitTagSetting)
{
	StatTags.AddStack(InTag, InStackCount);

	if (GetItemDefinition())
	{
		// Broadcast notify added item
		if (const UGmRIS_ItemFragment_NotificationInfo* NotificationFragment{
			FindFragmentByClass<UGmRIS_ItemFragment_NotificationInfo>()})
		{
			if ((NotificationFragment->NotificationBroadcastType == EGmNotificationType::EOnlyAdded ||
				NotificationFragment->NotificationBroadcastType == EGmNotificationType::BothAddedAndRemoved) &&
				!InIsInitTagSetting)
			{
				if (const AController* CurrentC{Cast<AController>(GetOuter())};
					CurrentC && CurrentC->HasAuthority())
				{
#if !UE_BUILD_SHIPPING
					// Debug Screen.
					UGmRIS_GlobalFuncLib::DebugPrintControllerRoles(CurrentC);
#endif
					if (UGmRIS_StorageComponent* StorageComponent{
						CurrentC->FindComponentByClass<UGmRIS_StorageComponent>()})
					{
						StorageComponent->Client_NotifyAddedEvent(NotificationFragment, InStackCount);
					}
				}
			}
		}//~Broadcast notify added item
	}
}

void UGmRIS_InventoryItemInstance::RemoveStatTagStack(const FGameplayTag InTag, const int32 InStackCount)
{
	StatTags.RemoveStack(InTag, InStackCount);

	if (GetItemDefinition())
	{
		// Broadcast notify added item
		if (const UGmRIS_ItemFragment_NotificationInfo* NotificationFragment{
			FindFragmentByClass<UGmRIS_ItemFragment_NotificationInfo>()})
		{
			if (NotificationFragment->NotificationBroadcastType == EGmNotificationType::EOnlyRemoved ||
				NotificationFragment->NotificationBroadcastType == EGmNotificationType::BothAddedAndRemoved)
			{
				if (const AController* CurrentC{Cast<AController>(GetOuter())};
					CurrentC && CurrentC->HasAuthority())
				{
#if !UE_BUILD_SHIPPING
					// Debug Screen.
					UGmRIS_GlobalFuncLib::DebugPrintControllerRoles(CurrentC);
#endif
					if (UGmRIS_StorageComponent* StorageComponent{CurrentC->FindComponentByClass<UGmRIS_StorageComponent>()})
					{
						StorageComponent->Client_NotifyRemovedEvent(NotificationFragment, InStackCount);
					}
				}
			}
		}//~Broadcast notify added item
	}
}

int32 UGmRIS_InventoryItemInstance::GetStatTagStackCount(const FGameplayTag InTag) const
{
	return StatTags.GetStackCount(InTag);
}

bool UGmRIS_InventoryItemInstance::HasStatTag(const FGameplayTag InTag) const
{
	return StatTags.ContainsTag(InTag);
}

const UGmRIS_InventoryItemFragment* UGmRIS_InventoryItemInstance::FindFragmentByClass(
	const TSubclassOf<UGmRIS_InventoryItemFragment> InFragmentClassRef) const
{
	return ItemDef && InFragmentClassRef ?
		GetDefault<UGmRIS_InventoryItemDefinition>(ItemDef)->FindFragmentByClass(InFragmentClassRef) : nullptr;
}

void UGmRIS_InventoryItemInstance::SetItemDefinition(const TSubclassOf<UGmRIS_InventoryItemDefinition> InDefinition)
{
	ItemDef = InDefinition;
}
