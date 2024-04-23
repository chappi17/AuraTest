// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "GameFramework/GameplayMsgTypes2.h"
#include "GameplayTagContainer.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include <UObject/WeakObjectPtr.h>

#include "GameplayMsgSubsystem.generated.h"

class UGameplayMsgSubsystem;
struct FFrame;

GAMEPLAYMSGRUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogGameplayMsgSubsystem, Log, All);

class UAsyncAction_ListenForGameplayMsg;

/**
 * An opaque handle that can be used to remove a previously registered message listener
 * @see UGameplayMsgSubsystem::RegisterListener and UGameplayMsgSubsystem::UnregisterListener
 */
USTRUCT(BlueprintType)
struct GAMEPLAYMSGRUNTIME_API FGameplayMsgListenerHandle
{
	
public:
	
	GENERATED_BODY()

	FGameplayMsgListenerHandle() {}

	void Unregister();

	bool IsValid() const { return ID != 0; }

private:
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UGameplayMsgSubsystem> Subsystem;

	UPROPERTY(Transient)
	FGameplayTag Channel;

	UPROPERTY(Transient)
	int32 ID{0};

	FDelegateHandle StateClearedHandle;

	friend UGameplayMsgSubsystem;

	FGameplayMsgListenerHandle(UGameplayMsgSubsystem* InSubsystem, const FGameplayTag InChannel, const int32 InID) : Subsystem(InSubsystem), Channel(InChannel), ID(InID) {}
};

/** 
 * Entry information for a single registered listener
 */
USTRUCT()
struct FGameplayMsgListenerData
{
	GENERATED_BODY()

	// Callback for when a message has been received
	TFunction<void(FGameplayTag, const UScriptStruct*, const void*)> ReceivedCallback;

	int32 HandleID;
	EGameplayMsgMatch MatchType;

	// Adding some logging and extra variables around some potential problems with this
	TWeakObjectPtr<const UScriptStruct> ListenerStructType{nullptr};
	bool bHadValidType{false};
};

/**
 * This system allows event raisers and listeners to register for messages without
 * having to know about each other directly, though they must agree on the format
 * of the message (as a USTRUCT() type).
 *
 *
 * You can get to the message router from the game instance:
 *    UGameInstance::GetSubsystem<UGameplayMsgSubsystem>(GameInstance)
 * or directly from anything that has a route to a world:
 *    UGameplayMsgSubsystem::Get(WorldContextObject)
 *
 * Note that call order when there are multiple listeners for the same channel is
 * not guaranteed and can change over time!
 */
UCLASS()
class GAMEPLAYMSGRUNTIME_API UGameplayMsgSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend UAsyncAction_ListenForGameplayMsg;

public:

	/**
	 * @return the message router for the game instance associated with the world of the specified object
	 */
	static UGameplayMsgSubsystem& Get(const UObject* WorldContextObject);

	/**
	 * @return true if a valid GameplayMsgRouter subsystem if active in the provided world
	 */
	static bool HasInstance(const UObject* WorldContextObject);

	//~USubsystem interface
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	/**
	 * Broadcast a message on the specified channel
	 *
	 * @param InChannel			The message channel to broadcast on
	 * @param InMsg			The message to send (must be the same type of UScriptStruct expected by the listeners for this channel, otherwise an error will be logged)
	 */
	template <typename FMessageStructType>
	void BroadcastMessage(FGameplayTag InChannel, const FMessageStructType& InMsg)
	{
		BroadcastMessageInternal(InChannel, /*Struct Type*/TBaseStructure<FMessageStructType>::Get(), &InMsg);
	}

	/**
	 * Register to receive messages on a specified channel
	 *
	 * @param Channel			The message channel to listen to
	 * @param Callback			Function to call with the message when someone broadcasts it (must be the same type of UScriptStruct provided by broadcasters for this channel, otherwise an error will be logged)
	 * @param MatchType			
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	template <typename FMessageStructType>
	FGameplayMsgListenerHandle RegisterListener(FGameplayTag Channel, TFunction<void(FGameplayTag, const FMessageStructType&)>&& Callback, EGameplayMsgMatch MatchType = EGameplayMsgMatch::ExactMatch)
	{
		auto ThunkCallback{[InnerCallback = MoveTemp(Callback)](FGameplayTag ActualTag, const UScriptStruct*/*SenderStructType*/, const void* SenderPayload)->void{InnerCallback(ActualTag, *reinterpret_cast<const FMessageStructType*>(SenderPayload));}};

		return RegisterListenerInternal(Channel, ThunkCallback, /*Struct Type*/TBaseStructure<FMessageStructType>::Get(), MatchType);
	}

	/**
	 * Register to receive messages on a specified channel and handle it with a specified member function
	 * Executes a weak object validity check to ensure the object registering the function still exists before triggering the callback
	 *
	 * @param Channel			The message channel to listen to
	 * @param Object			The object instance to call the function on
	 * @param Function			Member function to call with the message when someone broadcasts it (must be the same type of UScriptStruct provided by broadcasters for this channel, otherwise an error will be logged)
	 *
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	template <typename FMessageStructType, typename TOwner = UObject>
	FGameplayMsgListenerHandle RegisterListener(FGameplayTag Channel, TOwner* Object, void(TOwner::* Function)(FGameplayTag, const FMessageStructType&))
	{
		// UE_LOG(LogTemp, Error, L"Register Listener : Channel is %s, OwnerObject is %s.", *Channel.ToString(), *Object->GetName());
		TWeakObjectPtr<TOwner> WeakObject(Object);
		return RegisterListener<FMessageStructType>(Channel,
			[WeakObject, Function](FGameplayTag Channel, const FMessageStructType& Payload)->void
			{
				if (TOwner* StrongObject{WeakObject.Get()})
				{
					(StrongObject->*Function)(Channel, Payload);
				}
			});
	}

	/**
	 * Register to receive messages on a specified channel with extra parameters to support advanced behavior
	 * The stateful part of this logic should probably be separated out to a separate system
	 *
	 * @param Channel			The message channel to listen to
	 * @param Params			Structure containing details for advanced behavior
	 *
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	template <typename FMsgStructType>
	FGameplayMsgListenerHandle RegisterListener(FGameplayTag Channel, FGameplayMsgListenerParams<FMsgStructType>& Params)
	{
		FGameplayMsgListenerHandle Handle;

		// Register to receive any future messages broadcast on this channel
		if (Params.OnMessageReceivedCallback)
		{
			auto ThunkCallback{[InnerCallback = Params.OnMessageReceivedCallback](FGameplayTag ActualTag, const UScriptStruct*/*SenderStructType*/, const void* SenderPayload)->void
			{
				InnerCallback(ActualTag, *reinterpret_cast<const FMsgStructType*>(SenderPayload));
			}};

			Handle = RegisterListenerInternal(Channel, ThunkCallback, /*Struct Type*/TBaseStructure<FMsgStructType>::Get(), Params.MatchType);
		}

		return Handle;
	}

	/**
	 * Remove a message listener previously registered by RegisterListener
	 *
	 * @param InHandle	The handle returned by RegisterListener
	 */
	void UnregisterListener(const FGameplayMsgListenerHandle& InHandle);

protected:
	
	/**
	 * Broadcast a message on the specified channel
	 *
	 * @param InChannel			The message channel to broadcast on
	 * @param InMessage			The message to send (must be the same type of UScriptStruct expected by the listeners for this channel, otherwise an error will be logged)
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = Messaging, meta = (CustomStructureParam = "InMessage", AllowAbstract = "false", DisplayName = "Broadcast Message"))
	void K2_BroadcastMessage(FGameplayTag InChannel, const int32& InMessage);

	DECLARE_FUNCTION(execK2_BroadcastMessage);

private:
	
	// Internal helper for broadcasting a message
	void BroadcastMessageInternal(FGameplayTag InChannel, const UScriptStruct* StructType, const void* MessageBytes);

	// Internal helper for registering a message listener
	FGameplayMsgListenerHandle RegisterListenerInternal(
		FGameplayTag InChannel, 
		TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& InCallback,
		const UScriptStruct* InStructType,
		EGameplayMsgMatch InMatchType);

	void UnregisterListenerInternal(FGameplayTag InChannel, int32 InHandleID);
	
	// List of all entries for a given channel
	struct FChannelListenerList
	{
		TArray<FGameplayMsgListenerData> Listeners;
		int32 HandleID{0};
	};
	
	TMap<FGameplayTag, FChannelListenerList> ListenerMap;
	
};
