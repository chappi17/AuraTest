// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayMsgTypes2.generated.h"

class UGameplayMsgRouter;

// Match rule for message listeners
UENUM(BlueprintType)
enum class EGameplayMsgMatch : uint8
{
	ExactMatch,
	PartialMatch
};

/**
 * Struct used to specify advanced behavior when registering a listener for gameplay messages
 */
template<typename FMessageStructType>
struct FGameplayMsgListenerParams
{
	/** Whether Callback should be called for broadcasts of more derived channels or if it will only be called for exact matches. */
	EGameplayMsgMatch MatchType = EGameplayMsgMatch::ExactMatch;

	/** If bound this callback will trigger when a message is broadcast on the specified channel. */
	TFunction<void(FGameplayTag, const FMessageStructType&)> OnMessageReceivedCallback;

	/** Helper to bind weak member function to OnMessageReceivedCallback */
	template<typename TOwner = UObject>
	void SetMessageReceivedCallback(TOwner* Object, void(TOwner::* Function)(FGameplayTag, const FMessageStructType&))
	{
		TWeakObjectPtr<TOwner> WeakObject(Object);
		OnMessageReceivedCallback = [WeakObject, Function](FGameplayTag Channel, const FMessageStructType& Payload)
		{
			if (TOwner* StrongObject{WeakObject.Get()})
			{
				(StrongObject->*Function)(Channel, Payload);
			}
		};
	}
};
