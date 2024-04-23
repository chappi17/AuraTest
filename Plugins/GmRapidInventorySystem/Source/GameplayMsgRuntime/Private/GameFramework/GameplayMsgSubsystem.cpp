// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#include "GameFramework/GameplayMsgSubsystem.h"

#include <Engine/Engine.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <UObject/ScriptMacros.h>
#include <UObject/Stack.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayMsgSubsystem)

DEFINE_LOG_CATEGORY(LogGameplayMsgSubsystem);

namespace UE
{
	namespace GameplayMsgSubsystem
	{
		static int32 ShouldLogMessages{0};
		static FAutoConsoleVariableRef CVarShouldLogMessages(L"GameplayMsgSubsystem.LogMessages",
			ShouldLogMessages,
			L"Should messages broadcast through the gameplay message subsystem be logged?");
	}
}

//////////////////////////////////////////////////////////////////////
// FGameplayMsgListenerHandle

void FGameplayMsgListenerHandle::Unregister()
{
	if (UGameplayMsgSubsystem* StrongSubsystem{Subsystem.Get()})
	{
		StrongSubsystem->UnregisterListener(*this);
		Subsystem.Reset();
		Channel = FGameplayTag();
		ID = 0;
	}
}

//////////////////////////////////////////////////////////////////////
// UGameplayMsgSubsystem

UGameplayMsgSubsystem& UGameplayMsgSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World{GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)};
	check(World);
	UGameplayMsgSubsystem* Router{UGameInstance::GetSubsystem<UGameplayMsgSubsystem>(World->GetGameInstance())};
	check(Router);
	return *Router;
}

bool UGameplayMsgSubsystem::HasInstance(const UObject* WorldContextObject)
{
	const UWorld* World{GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)};
	const UGameplayMsgSubsystem* Router{World ? UGameInstance::GetSubsystem<UGameplayMsgSubsystem>(World->GetGameInstance()) : nullptr};
	return Router != nullptr;
}

void UGameplayMsgSubsystem::Deinitialize()
{
	ListenerMap.Reset();

	Super::Deinitialize();
}

void UGameplayMsgSubsystem::BroadcastMessageInternal(const FGameplayTag InChannel, const UScriptStruct* StructType, const void* MessageBytes)
{
	// Log the message if enabled
	if (UE::GameplayMsgSubsystem::ShouldLogMessages != 0)
	{
		const FString* pContextString{nullptr};
#if WITH_EDITOR
		if (GIsEditor)
		{
			extern ENGINE_API FString GPlayInEditorContextString;
			pContextString = &GPlayInEditorContextString;
		}
#endif

		FString HumanReadableMessage;
		StructType->ExportText(/*out*/ HumanReadableMessage, MessageBytes, /*Defaults=*/ nullptr, /*OwnerObject=*/ nullptr, PPF_None, /*ExportRootScope=*/ nullptr);
		UE_LOG(LogGameplayMsgSubsystem, Log, L"BroadcastMessage(%s, %s, %s)", pContextString ? **pContextString : *GetPathNameSafe(this), *InChannel.ToString(), *HumanReadableMessage);
	}

	// Broadcast the message
	bool bOnInitialTag{true};
	for (FGameplayTag Tag{InChannel}; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const FChannelListenerList* pList{ListenerMap.Find(Tag)})
		{
			// Copy in case there are removals while handling callbacks
			TArray/*<FGameplayMsgListenerData>*/ ListenerArray(pList->Listeners);

			for (const FGameplayMsgListenerData& Listener : ListenerArray)
			{
				if (bOnInitialTag || Listener.MatchType == EGameplayMsgMatch::PartialMatch)
				{
					if (Listener.bHadValidType && !Listener.ListenerStructType.IsValid())
					{
						UE_LOG(LogGameplayMsgSubsystem, Warning, L"Listener struct type has gone invalid on Channel %s. Removing listener from list", *InChannel.ToString());
						UnregisterListenerInternal(InChannel, Listener.HandleID);
						continue;
					}

					// The receiving type must be either a parent of the sending type or completely ambiguous (for internal use)
					if (!Listener.bHadValidType || StructType->IsChildOf(Listener.ListenerStructType.Get()))
					{
						Listener.ReceivedCallback(InChannel, StructType, MessageBytes);
					}
					else
					{
						UE_LOG(LogGameplayMsgSubsystem, Error, L"Struct type mismatch on channel %s (broadcast type %s, listener at %s was expecting %s)",
							*InChannel.ToString(),
							*StructType->GetPathName(),
							*Tag.ToString(),
							*Listener.ListenerStructType->GetPathName());
					}
				}
			}
		}
		bOnInitialTag = false;
	}
}

void UGameplayMsgSubsystem::K2_BroadcastMessage(FGameplayTag, const int32&)
{
	// This will never be called, the exec version below will be hit instead
	checkNoEntry();
}

DEFINE_FUNCTION(UGameplayMsgSubsystem::execK2_BroadcastMessage)
{
	P_GET_STRUCT(FGameplayTag, Channel);

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const void* MessagePtr{Stack.MostRecentPropertyAddress};
	const FStructProperty* StructProp{CastField<FStructProperty>(Stack.MostRecentProperty)};

	P_FINISH;

	if (ensure((StructProp != nullptr) && (StructProp->Struct != nullptr) && (MessagePtr != nullptr)))
	{
		P_THIS->BroadcastMessageInternal(Channel, StructProp->Struct, MessagePtr);
	}
}

FGameplayMsgListenerHandle UGameplayMsgSubsystem::RegisterListenerInternal(const FGameplayTag InChannel, TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& InCallback, const UScriptStruct* InStructType, const EGameplayMsgMatch InMatchType)
{
	auto& [Elem_Listeners, Elem_HandleID]{ListenerMap.FindOrAdd(InChannel)};

	auto& [ReceivedCallback, HandleID, MatchType, ListenerStructType, bHadValidType]{Elem_Listeners.AddDefaulted_GetRef()};
	ReceivedCallback = MoveTemp(InCallback);
	ListenerStructType = InStructType;
	bHadValidType = static_cast<bool>(InStructType);
	HandleID = ++Elem_HandleID;
	MatchType = InMatchType;

	return FGameplayMsgListenerHandle(this, InChannel, HandleID);
}

void UGameplayMsgSubsystem::UnregisterListener(const FGameplayMsgListenerHandle& InHandle)
{
	if (InHandle.IsValid())
	{
		check(InHandle.Subsystem == this);

		UnregisterListenerInternal(InHandle.Channel, InHandle.ID);
	}
	else
	{
		UE_LOG(LogGameplayMsgSubsystem, Warning, L"Trying to unregister an invalid Handle.");
	}
}

void UGameplayMsgSubsystem::UnregisterListenerInternal(const FGameplayTag InChannel, const int32 InHandleID)
{
	if (FChannelListenerList* pList{ListenerMap.Find(InChannel)})
	{
		if (const int32 MatchIndex{pList->Listeners.IndexOfByPredicate([ID = InHandleID](const FGameplayMsgListenerData& Other)->bool { return Other.HandleID == ID; })}; MatchIndex != INDEX_NONE)
		{
			pList->Listeners.RemoveAtSwap(MatchIndex);
		}

		if (pList->Listeners.Num() == 0)
		{
			ListenerMap.Remove(InChannel);
		}
	}
}
