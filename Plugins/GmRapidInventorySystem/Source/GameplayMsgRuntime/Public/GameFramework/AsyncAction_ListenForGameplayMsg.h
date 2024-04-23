// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayMsgSubsystem.h"
#include "GameplayMsgTypes2.h"

#include "AsyncAction_ListenForGameplayMsg.generated.h"

class UScriptStruct;
class UWorld;
struct FFrame;

/**
 * Proxy object pin will be hidden in K2Node_GameplayMsgAsyncAction. Is used to get a reference to the object triggering the delegate for the follow up call of 'GetPayload'.
 *
 * @param ActualChannel		The actual message channel that we received Payload from (will always start with Channel, but may be more specific if partial matches were enabled)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncGameplayMsgDelegate, UAsyncAction_ListenForGameplayMsg*, ProxyObject, FGameplayTag, ActualChannel);

UCLASS(BlueprintType, meta = (HasDedicatedAsyncNode))
class GAMEPLAYMSGRUNTIME_API UAsyncAction_ListenForGameplayMsg : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	
	/**
	 * Asynchronously waits for a gameplay message to be broadcast on the specified channel.
	 *
	 * @param Channel			The message channel to listen for
	 * @param PayloadType		The kind of message structure to use (this must match the same type that the sender is broadcasting)
	 * @param MatchType			The rule used for matching the channel with broadcasted messages
	 */
	UFUNCTION(BlueprintCallable, Category = Messaging, meta = (WorldContext = "WorldContextObj", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_ListenForGameplayMsg* ListenForGameplayMsgs(UObject* WorldContextObj, FGameplayTag Channel, UScriptStruct* PayloadType, EGameplayMsgMatch MatchType = EGameplayMsgMatch::ExactMatch);

	/**
	 * Attempt to copy the payload received from the broadcasted gameplay message into the specified wildcard.
	 * The wildcard's type must match the type from the received message.
	 *
	 * @param OutPayload	The wildcard reference the payload should be copied into
	 * @return				If the copy was a success
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Messaging", meta = (CustomStructureParam = "OutPayload"))
	bool GetPayload(UPARAM(ref) int32& OutPayload);

	DECLARE_FUNCTION(execGetPayload);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

public:
	
	/** Called when a message is broadcast on the specified channel. Use GetPayload() to request the message payload. */
	UPROPERTY(BlueprintAssignable)
	FAsyncGameplayMsgDelegate OnMessageReceived;

private:
	
	void HandleMessageReceived(FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload);
	
	const void* ReceivedMessagePayloadPtr = nullptr;

	TWeakObjectPtr<UWorld> WorldPtr;
	FGameplayTag ChannelToRegister;
	TWeakObjectPtr<UScriptStruct> MessageStructType = nullptr;
	EGameplayMsgMatch MessageMatchType = EGameplayMsgMatch::ExactMatch;

	FGameplayMsgListenerHandle ListenerHandle;
	
};
