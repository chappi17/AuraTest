// Copyright 2024 Dev.GaeMyo. All Rights Reserved.


#include "Equipment/GmRIS_EquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "Equipment/GmRIS_EquipmentInstance.h"
#include "Equipment/Item/Definition/GmRIS_EquipmentDefinition.h"
#include "Net/UnrealNetwork.h"

class FLifetimeProperty;
struct FReplicationFlags;

FString FGmRISAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(L"%s of %s", *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

void FGmRISEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32)
{
	for (const int32 Index : RemovedIndices)
	{
		if (const FGmRISAppliedEquipmentEntry& Entry{Entries[Index]}; Entry.Instance)
		{
			Entry.Instance->OnUnequipped();
		}
	}
}

void FGmRISEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32)
{
	for (const int32 Index : AddedIndices)
	{
		if (const FGmRISAppliedEquipmentEntry& Entry{Entries[Index]}; Entry.Instance)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void FGmRISEquipmentList::PostReplicatedChange(const TArrayView<int32>, int32)
{
	// 	for (const int32 Index : ChangedIndices)
	// 	{
	// 		const FGameplayTagStack& Stack{Stacks[Index]};
	// 		TagToCountMap[Stack.Tag]{Stack.StackCount};
	// 	}
}

UGmRIS_EquipmentInstance* FGmRISEquipmentList::AddEntry(const TSubclassOf<UGmRIS_EquipmentDefinition> InEquipmentDefClass)
{
	check(InEquipmentDefClass);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	
	const UGmRIS_EquipmentDefinition* EquipmentCDO{GetDefault<UGmRIS_EquipmentDefinition>(InEquipmentDefClass)};

	TSubclassOf/*<UGmRIS_EquipmentInstance>*/ InstanceType{EquipmentCDO->InstanceType};
	if (!InstanceType)
	{
		InstanceType = UGmRIS_EquipmentInstance::StaticClass();
	}
	
	FGmRISAppliedEquipmentEntry& NewEntry{Entries.AddDefaulted_GetRef()};
	NewEntry.EquipmentDefinition = InEquipmentDefClass;
	NewEntry.Instance = NewObject<UGmRIS_EquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	UGmRIS_EquipmentInstance* Result = NewEntry.Instance;

	// if (UAbilitySystemComponent* ASC{GetAbilitySystemComponent()})
	// {
		// for (TObjectPtr<const UGmRISAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		// {
			// AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		// }
	// }
	// else
	// {
		// @TODO: Warning logging?
	// }

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);


	MarkItemDirty(NewEntry);

	return Result;
}

void FGmRISEquipmentList::RemoveEntry(UGmRIS_EquipmentInstance* InInst)
{
	for (auto EntryIt{Entries.CreateIterator()}; EntryIt; ++EntryIt)
	{
		if (FGmRISAppliedEquipmentEntry& Entry{*EntryIt}; Entry.Instance == InInst)
		{
			//@TODO Add “UGmRISAbilitySystemComponent”.
			// if (UAbilitySystemComponent* ASC{GetAbilitySystemComponent()})
			// {
				// Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			// }

			InInst->DestroyEquipmentActors();
			
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

UAbilitySystemComponent* FGmRISEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	const AActor* OwningActor{OwnerComponent->GetOwner()};
	//@TODO Add “UGmRISAbilitySystemComponent”.
	return Cast</*UGmRISAbilitySystemComponent*/UAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

UGmRIS_EquipmentManagerComponent::UGmRIS_EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
	bWantsInitializeComponent = true;
}

void UGmRIS_EquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

UGmRIS_EquipmentInstance* UGmRIS_EquipmentManagerComponent::EquipItem(
	const TSubclassOf<UGmRIS_EquipmentDefinition> InEquipmentDefType)
{
	UGmRIS_EquipmentInstance* Loc_Result{nullptr};
	if (!InEquipmentDefType)
	{
		Loc_Result = EquipmentList.AddEntry(InEquipmentDefType);
		if (Loc_Result != nullptr)
		{
			Loc_Result->OnEquipped();

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Loc_Result);
			}
		}
	}
	return Loc_Result;
}

void UGmRIS_EquipmentManagerComponent::UnEquipItem(UGmRIS_EquipmentInstance* InItemInst)
{
	if (InItemInst)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(InItemInst);
		}

		InItemInst->OnUnequipped();
		EquipmentList.RemoveEntry(InItemInst);
	}
}

bool UGmRIS_EquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool WroteSomething{Super::ReplicateSubobjects(Channel, Bunch, RepFlags)};

	for (FGmRISAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UGmRIS_EquipmentInstance* Instance{Entry.Instance};

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}

void UGmRIS_EquipmentManagerComponent::UninitializeComponent()
{
	TArray<UGmRIS_EquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FGmRISAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UGmRIS_EquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnEquipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

void UGmRIS_EquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing LyraEquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FGmRISAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			if (UGmRIS_EquipmentInstance* Instance{Entry.Instance}; IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

UGmRIS_EquipmentInstance* UGmRIS_EquipmentManagerComponent::GetFirstInstanceOfType(
	const TSubclassOf<UGmRIS_EquipmentInstance> InInstanceType)
{
	for (FGmRISAppliedEquipmentEntry& ElemEntry : EquipmentList.Entries)
	{
		if (UGmRIS_EquipmentInstance* ElemInst{ElemEntry.Instance})
		{
			if (ElemInst->IsA(InInstanceType))
			{
				return ElemInst;
			}
		}
	}
	return nullptr;
}

TArray<UGmRIS_EquipmentInstance*> UGmRIS_EquipmentManagerComponent::GetEquipmentInstancesOfType(
	const TSubclassOf<UGmRIS_EquipmentInstance> InInstanceType) const
{
	TArray<UGmRIS_EquipmentInstance*> Results;
	for (const FGmRISAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UGmRIS_EquipmentInstance* Instance{Entry.Instance})
		{
			if (Instance->IsA(InInstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}
