// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Core/GmGameplayTagStack.h"

#include "Logging/LogVerbosity.h"
#include "UObject/Stack.h"

// FGmGameplayTagStack
FString FGmGameplayTagStack::GetDebugString() const
{
	return FString::Printf(L"%sx%d", *Tag.ToString(), StackCount);
}

// FGmGameplayTagStackContainer
void FGmGameplayTagStackContainer::AddStack(FGameplayTag InTag, int32 InStackCount)
{
	if (!InTag.IsValid())
	{
		FFrame::KismetExecutionMessage(L"An invalid tag was passed to AddStack", ELogVerbosity::Warning);
		return;
	}

	if (InStackCount > 0)
	{
		for (FGmGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == InTag)
			{
				const int32 NewCount{Stack.StackCount + InStackCount};
				Stack.StackCount = NewCount;
				TagToCountMap[InTag] = NewCount;
				MarkItemDirty(Stack);
				return;
			}
		}

		MarkItemDirty(/*NewStack*/Stacks.Emplace_GetRef(InTag, InStackCount));
		TagToCountMap.Add(InTag, InStackCount);
	}
}

void FGmGameplayTagStackContainer::RemoveStack(const FGameplayTag InTag, const int32 InStackCount)
{
	if (!InTag.IsValid())
	{
		FFrame::KismetExecutionMessage(L"An invalid tag was passed to RemoveStack", ELogVerbosity::Warning);
		return;
	}

	//@TODO: Should we error if you try to remove a stack that doesn't exist or has a smaller count?
	if (InStackCount > 0)
	{
		for (auto It{Stacks.CreateIterator()}; It; ++It)
		{
			if (FGmGameplayTagStack& Stack{*It}; Stack.Tag == InTag)
			{
				if (Stack.StackCount <= InStackCount)
				{
					It.RemoveCurrent();
					TagToCountMap.Remove(InTag);
					MarkArrayDirty();
				}
				else
				{
					const int32 NewCount{Stack.StackCount - InStackCount};
					Stack.StackCount = NewCount;
					TagToCountMap[InTag] = NewCount;
					MarkItemDirty(Stack);
				}
				return;
			}
		}
	}
}

void FGmGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32)
{
	for (const int32 Index : InRemovedIndices)
	{
		TagToCountMap.Remove(Stacks[Index].Tag);
	}
}

void FGmGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32)
{
	for (const int32 Index : InAddedIndices)
	{
		const FGmGameplayTagStack& Stack{Stacks[Index]};
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

void FGmGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32)
{
	for (const int32 Index : InChangedIndices)
	{
		const FGmGameplayTagStack& Stack{Stacks[Index]};
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}
