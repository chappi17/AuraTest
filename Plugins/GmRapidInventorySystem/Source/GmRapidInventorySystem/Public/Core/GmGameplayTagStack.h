// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/ArrayView.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include "Containers/SparseArray.h"
#include "Containers/UnrealString.h"
#include "GameplayTagContainer.h"
#include "HAL/Platform.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Templates/UnrealTemplate.h"
#include "UObject/Class.h"

#include "GmGameplayTagStack.generated.h"

struct FGmGameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

/**
 * Represents one stack of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct FGmGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGmGameplayTagStack()
	{}

	FGmGameplayTagStack(const FGameplayTag InTag, const int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{
	}

	FString GetDebugString() const;

private:
	
	friend FGmGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount{0};
};

/** Container of gameplay tag stacks */
USTRUCT(BlueprintType)
struct FGmGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FGmGameplayTagStackContainer()
	{}

public:
	
	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	void AddStack(FGameplayTag InTag, int32 InStackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	void RemoveStack(FGameplayTag InTag, int32 InStackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	int32 GetStackCount(const FGameplayTag InTag) const
	{
		return TagToCountMap.FindRef(InTag);
	}

	// Returns true if there is at least one stack of the specified tag
	bool ContainsTag(const FGameplayTag InTag) const
	{
		return TagToCountMap.Contains(InTag);
	}

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize);
	void PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize);
	void PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& OutDeltaParams)
	{
		return /*FFastArraySerializer::*/FastArrayDeltaSerialize<FGmGameplayTagStack, FGmGameplayTagStackContainer>(Stacks, OutDeltaParams, *this);
	}

private:
	
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FGmGameplayTagStack> Stacks;
	
	// Accelerated list of tag stacks for queries
	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FGmGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGmGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
