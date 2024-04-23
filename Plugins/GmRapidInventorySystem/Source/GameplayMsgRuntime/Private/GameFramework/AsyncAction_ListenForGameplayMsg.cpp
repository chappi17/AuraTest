// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#include "GameFramework/AsyncAction_ListenForGameplayMsg.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMsgSubsystem.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ListenForGameplayMsg)

UAsyncAction_ListenForGameplayMsg* UAsyncAction_ListenForGameplayMsg::ListenForGameplayMsgs(UObject* WorldContextObj, FGameplayTag Channel, UScriptStruct* PayloadType, EGameplayMsgMatch MatchType)
{
	UWorld* World{GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull)};
	if (!World)
	{
		return nullptr;
	}

	UAsyncAction_ListenForGameplayMsg* Action{NewObject<UAsyncAction_ListenForGameplayMsg>()};
	Action->WorldPtr = World;
	Action->ChannelToRegister = Channel;
	Action->MessageStructType = PayloadType;
	Action->MessageMatchType = MatchType;
	Action->RegisterWithGameInstance(World);

	return Action;
}

void UAsyncAction_ListenForGameplayMsg::Activate()
{
	if (const UWorld* World{WorldPtr.Get()})
	{
		if (UGameplayMsgSubsystem::HasInstance(World))
		{
			UGameplayMsgSubsystem& Router{UGameplayMsgSubsystem::Get(World)};

			TWeakObjectPtr<UAsyncAction_ListenForGameplayMsg> WeakThis(this);
			ListenerHandle = Router.RegisterListenerInternal(ChannelToRegister,
				[WeakThis](FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload)->void
				{
					if (UAsyncAction_ListenForGameplayMsg* StrongThis{WeakThis.Get()})
					{
						StrongThis->HandleMessageReceived(Channel, StructType, Payload);
					}
				},
				MessageStructType.Get(),
				MessageMatchType);

			return;
		}
	}

	SetReadyToDestroy();
}

void UAsyncAction_ListenForGameplayMsg::SetReadyToDestroy()
{
	ListenerHandle.Unregister();

	Super::SetReadyToDestroy();
}

bool UAsyncAction_ListenForGameplayMsg::GetPayload(int32&)
{
	checkNoEntry();
	return false;
}

DEFINE_FUNCTION(UAsyncAction_ListenForGameplayMsg::execGetPayload)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* MessagePtr{Stack.MostRecentPropertyAddress};
	const FStructProperty* StructProp{CastField<FStructProperty>(Stack.MostRecentProperty)};
	P_FINISH;

	bool bSuccess{false};

	// 블루프린트 노드를 통해 얻으려는 유형이 수신된 메시지 페이로드 유형과 일치하는지 확인하세요.
	// Make sure the type we are trying to get through the blueprint node matches the type of the message payload received.
	if (StructProp && StructProp->Struct && MessagePtr && StructProp->Struct == P_THIS->MessageStructType.Get() && P_THIS->ReceivedMessagePayloadPtr)
	{
		StructProp->Struct->CopyScriptStruct(MessagePtr, P_THIS->ReceivedMessagePayloadPtr);
		bSuccess = true;
	}

	*(bool*)RESULT_PARAM = bSuccess;
}

void UAsyncAction_ListenForGameplayMsg::HandleMessageReceived(FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload)
{
	if (!MessageStructType.Get() || (MessageStructType.Get() == StructType))
	{
		ReceivedMessagePayloadPtr = Payload;

		OnMessageReceived.Broadcast(this, Channel);

		ReceivedMessagePayloadPtr = nullptr;
	}

	if (!OnMessageReceived.IsBound())
	{
		// If the BP object that created the async node is destroyed, OnMessageReceived will be unbound after calling the broadcast.
		// In this case we can safely mark this receiver as ready for destruction.
		// Need to support a more proactive mechanism for cleanup FORT-340994
		SetReadyToDestroy();
	}
}
