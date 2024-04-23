// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GmRIS_EquipmentManagerComponent.generated.h"

class UActorComponent;
class UAbilitySystemComponent;
class UGmRIS_EquipmentDefinition;
class UGmRIS_EquipmentInstance;
class UGmRIS_EquipmentManagerComponent;
class UObject;
struct FFrame;
struct FGmRISEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FGmRISAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGmRISAppliedEquipmentEntry()
		:
	EquipmentDefinition(nullptr),
	Instance(nullptr)
	{}

	// FGmRISAppliedEquipmentEntry(TSubclassOf<UGmRIS_EquipmentDefinition> InEquipmentDefClass, UGmRIS_EquipmentInstance* InInst)
	// 	:
	// EquipmentDefinition(InEquipmentDefClass),
	// Instance(InInst)
	// {}

	FString GetDebugString() const;

private:
	friend FGmRISEquipmentList;
	friend UGmRIS_EquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UGmRIS_EquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UGmRIS_EquipmentInstance> Instance;

	//@TODO Create "FGmRISAbilitySet_GrantedHandles" and upgrade its extension.
	// Authority-only list of granted handles
	// UPROPERTY(NotReplicated)
	// FGmRISAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FGmRISEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGmRISEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FGmRISEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGmRISAppliedEquipmentEntry, FGmRISEquipmentList>(Entries, DeltaParams, *this);
	}

	UGmRIS_EquipmentInstance* AddEntry(TSubclassOf<UGmRIS_EquipmentDefinition> InEquipmentDefClass);
	void RemoveEntry(UGmRIS_EquipmentInstance* InInst);

private:
	//@TODO Extended replacement of "UAbilitySystemComponent" with "UGmRISAbilitySystemComponent".
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UGmRIS_EquipmentManagerComponent;

	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FGmRISAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FGmRISEquipmentList> : public TStructOpsTypeTraitsBase2<FGmRISEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};


UCLASS(BlueprintType, Const, meta = (BlueprintSpawnableComponent))
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_EquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UGmRIS_EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GmRISInventory)
	UGmRIS_EquipmentInstance* EquipItem(UPARAM(DisplayName = "Equipment Definition Class") TSubclassOf<UGmRIS_EquipmentDefinition> InEquipmentDefType);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GmRISInventory)
	void UnEquipItem(UGmRIS_EquipmentInstance* InItemInst);

	//~UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override{ Super::InitializeComponent(); }
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GmRISInventory)
	UGmRIS_EquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UGmRIS_EquipmentInstance> InInstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GmRISInventory)
	TArray<UGmRIS_EquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UGmRIS_EquipmentInstance> InInstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}
	
private:

	UPROPERTY(Replicated)
	FGmRISEquipmentList EquipmentList;
	
};
