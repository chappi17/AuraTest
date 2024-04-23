// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GmRIS_InventoryItemInstance.h"
#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMsgSubsystem.h"
#include "Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GmRIS_StorageComponent.generated.h"

class UGmRIS_ItemFragment_InventoryTile;
class UGmRISW_InventoryContainer;
class UGmRIS_InventoryItemDefinition;
class UGmRIS_InventoryItemInstance;
class UObject;
class UGmRIS_StorageComponent;
struct FGmRISInventoryList;
struct FFrame;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FGmRISInventoryChangeMsg
{
	GENERATED_BODY()

	FGmRISInventoryChangeMsg()
		:
	InventoryOwner(nullptr),
	Instance(nullptr),
	NewCount(0),
	Delta(0),
	UniqueStorageID()
	{}
	
	FGmRISInventoryChangeMsg(UActorComponent* InNewInventoryOwner, UGmRIS_InventoryItemInstance* InNewInstance, const int32 InNewCount, const int32 InNewDelta, const uint32 InUniqueOwnerID = 0)
		:
	InventoryOwner(InNewInventoryOwner),
	Instance(InNewInstance),
	NewCount(InNewCount),
	Delta(InNewDelta),
	UniqueStorageID(InUniqueOwnerID)
	{}
	
	FORCEINLINE bool CanApplyInteraction(const uint32 InUniqueIDForComparison) const
	{
		return UniqueStorageID != 0 && InUniqueIDForComparison == UniqueStorageID;
	}

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UActorComponent> InventoryOwner;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UGmRIS_InventoryItemInstance> Instance;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NewCount;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 Delta;

private:

	uint32 UniqueStorageID;
};

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FGmRISInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGmRISInventoryEntry()
		:
	Instance(nullptr),
	StackCount(0),
	LastObservedCount(INDEX_NONE)
	{}

	FString GetDebugString() const;

private:
	
	friend FGmRISInventoryList;
	friend UGmRIS_StorageComponent;

	UPROPERTY()
	TObjectPtr<UGmRIS_InventoryItemInstance> Instance;

	UPROPERTY()
	int32 StackCount;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FGmRISInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGmRISInventoryList()
		:OwnerComponent(nullptr)
	{}

	FGmRISInventoryList(UActorComponent* InOwnerComp)
		:
	OwnerComponent(InOwnerComp)
	{}
	
	TArray<UGmRIS_InventoryItemInstance*> GetAllItems() const;

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize);
	void PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize);
	void PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& OutDeltaParams)
	{
		return /*FFastArraySerializer::*/FastArrayDeltaSerialize<FGmRISInventoryEntry, FGmRISInventoryList>(Entries, OutDeltaParams, *this);
	}

	UGmRIS_InventoryItemInstance* AddEntry(TSubclassOf<UGmRIS_InventoryItemDefinition> InItemClass, int32 InStackCount);
	// Only use if you pick up the item again
	void AddEntry(UGmRIS_InventoryItemInstance* InInstance);
	void RemoveEntry(UGmRIS_InventoryItemInstance* InInstance);
	// bool SwapEntry(UGmRIS_InventoryItemInstance* InTargetA, UGmRIS_InventoryItemInstance* InTargetB);

private:

	void BroadcastChangeMsg(const FGmRISInventoryEntry& OutEntry, const int32 InOldCount, const int32 InNewCount) const;

	friend UGmRIS_StorageComponent;
	
	// Replicated list of items
	UPROPERTY()
	TArray<FGmRISInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FGmRISInventoryList> : public TStructOpsTypeTraitsBase2<FGmRISInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};


/**
 * Manages an inventory.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_StorageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UGmRIS_StorageComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings|Debug", meta = (ExposeOnSpawn = true))
	bool bIsDebug{false};
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemDefinition(UPARAM(DisplayName = "Item Definition") TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef, int32 InStackCount = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	UGmRIS_InventoryItemInstance* AddItemDefinition(UPARAM(DisplayName = "Item Definition") TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef, int32 InStackCount = 1, const bool InIsInitInventorySetting = false);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddItemInstance(UPARAM(DisplayName = "Item Instance") UGmRIS_InventoryItemInstance* InItemInstance, const bool InIsInitInventorySetting = false);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveItemInstance(UPARAM(DisplayName = "Item Instance") UGmRIS_InventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	bool HasItemStackByItemInst(const UGmRIS_InventoryItemInstance* InItemInstToFind);
	
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure = false)
	TArray<UGmRIS_InventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	UGmRIS_InventoryItemInstance* FindFirstItemStackByDefinition(UPARAM(DisplayName = "Item Definition") const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SwapItem(UGmRIS_InventoryItemInstance* InTargetA, UGmRIS_InventoryItemInstance* InTargetB);

	int32 GetTotalItemCountByDefinition(const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef) const;

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

	// Notifications
	UFUNCTION(Client, Unreliable)
	void Client_NotifyAddedEvent(const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InAddedCount);

	UFUNCTION(Client, Unreliable)
	void Client_NotifyRemovedEvent(const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InRemovedCount);
	
protected:

	UPROPERTY(Replicated)
	FGmRISInventoryList Storage;

private:
	
	virtual void Internal_ClientNotifyAddedEvent(const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InAddedCount) const;
	virtual void Internal_ClientNotifyRemovedEvent(const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InRemovedCount) const;
};
