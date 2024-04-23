// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#include "G2Node_AsyncAction_ListenForGameplayMsgs.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "GameFramework/AsyncAction_ListenForGameplayMsg.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_AsyncAction.h"
#include "K2Node_CallFunction.h"
#include "K2Node_TemporaryVariable.h"
#include "KismetCompiler.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(G2Node_AsyncAction_ListenForGameplayMsgs)

class UEdGraph;

#define LOCTEXT_NAMESPACE "G2Node"

namespace UG2Node_AsyncAction_ListenForGameplayMsgsHelper
{
	static FName ActualChannelPinName = "ActualChannel";
	static FName PayloadPinName = "Payload";
	static FName PayloadTypePinName = "PayloadType";
	static FName DelegateProxyPinName = "ProxyObject";
};

void UG2Node_AsyncAction_ListenForGameplayMsgs::PostReconstructNode()
{
	Super::PostReconstructNode();

	RefreshOutputPayloadType();
}

void UG2Node_AsyncAction_ListenForGameplayMsgs::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	if (ChangedPin == GetPayloadTypePin())
	{
		if (ChangedPin->LinkedTo.Num() == 0)
		{
			RefreshOutputPayloadType();
		}
	}
}

void UG2Node_AsyncAction_ListenForGameplayMsgs::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);
	if (Pin.PinName == UG2Node_AsyncAction_ListenForGameplayMsgsHelper::PayloadPinName)
	{
		HoverTextOut = HoverTextOut + LOCTEXT("PayloadOutTooltip", "\n\nThe message structure that we received").ToString();
	}
}

void UG2Node_AsyncAction_ListenForGameplayMsgs::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct FGetMenuActions_Utils
	{
		static void SetNodeFunc(UEdGraphNode* NewNode, bool /*bIsTemplateNode*/, TWeakObjectPtr<UFunction> FunctionPtr)
		{
			UG2Node_AsyncAction_ListenForGameplayMsgs* AsyncTaskNode{CastChecked<UG2Node_AsyncAction_ListenForGameplayMsgs>(NewNode)};
			if (FunctionPtr.IsValid())
			{
				const UFunction* Func{FunctionPtr.Get()};
				const FObjectProperty* ReturnProp{CastFieldChecked<FObjectProperty>(Func->GetReturnProperty())};
						
				AsyncTaskNode->ProxyFactoryFunctionName = Func->GetFName();
				AsyncTaskNode->ProxyFactoryClass        = Func->GetOuterUClass();
				AsyncTaskNode->ProxyClass               = ReturnProp->PropertyClass;
			}
		}
	};

	UClass* NodeClass{GetClass()};
	ActionRegistrar.RegisterClassFactoryActions<UAsyncAction_ListenForGameplayMsg>(FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateLambda([NodeClass](const UFunction* FactoryFunc)->UBlueprintNodeSpawner*
	{
		UBlueprintNodeSpawner* NodeSpawner{UBlueprintFunctionNodeSpawner::Create(FactoryFunc)};
		check(NodeSpawner);
		NodeSpawner->NodeClass = NodeClass;

		const TWeakObjectPtr/*<UFunction>*/ FunctionPtr{MakeWeakObjectPtr(const_cast<UFunction*>(FactoryFunc))};
		NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(FGetMenuActions_Utils::SetNodeFunc, FunctionPtr);

		return NodeSpawner;
	}));
}

void UG2Node_AsyncAction_ListenForGameplayMsgs::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// The output of the UAsyncAction_ListenForGameplayMsg delegates is a proxy object which is used in the follow up call of GetPayload when triggered
	// This is only needed in the internals of this node so hide the pin from the editor.
	if (UEdGraphPin* DelegateProxyPin{FindPin(UG2Node_AsyncAction_ListenForGameplayMsgsHelper::DelegateProxyPinName)}; ensure(DelegateProxyPin))
	{
		DelegateProxyPin->bHidden = true;
	}

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UG2Node_AsyncAction_ListenForGameplayMsgsHelper::PayloadPinName);
}

bool UG2Node_AsyncAction_ListenForGameplayMsgs::HandleDelegates(const TArray<FBaseAsyncTaskHelper::FOutputPinAndLocalVariable>& VariableOutputs, UEdGraphPin* ProxyObjectPin, UEdGraphPin*& InOutLastThenPin, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext)
{
	bool bIsErrorFree{true};

	if (VariableOutputs.Num() != 3)
	{
		ensureMsgf(false, L"UG2Node_AsyncAction_ListenForGameplayMsgs::HandleDelegates - Variable output array not valid. Output delegates must only have the single proxy object output and than must have pin for payload.");
		return false;
	}

	for (TFieldIterator<FMulticastDelegateProperty> PropertyIt(ProxyClass); PropertyIt && bIsErrorFree; ++PropertyIt)
	{
		UEdGraphPin* LastActivatedThenPin{nullptr};
		bIsErrorFree &= FBaseAsyncTaskHelper::HandleDelegateImplementation(*PropertyIt, VariableOutputs, ProxyObjectPin, InOutLastThenPin, LastActivatedThenPin, this, SourceGraph, CompilerContext);

		bIsErrorFree &= HandlePayloadImplementation(*PropertyIt, VariableOutputs[0], VariableOutputs[2], VariableOutputs[1], LastActivatedThenPin, SourceGraph, CompilerContext);
	}

	return bIsErrorFree;
}

bool UG2Node_AsyncAction_ListenForGameplayMsgs::HandlePayloadImplementation(FMulticastDelegateProperty* CurrentProperty, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& ProxyObjectVar, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& PayloadVar, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& ActualChannelVar, UEdGraphPin*& InOutLastActivatedThenPin, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext)
{
	bool bIsErrorFree{true};
	const UEdGraphPin* PayloadPin{GetPayloadPin()};
	const UEdGraphSchema_K2* Schema{CompilerContext.GetSchema()};

	check(CurrentProperty && SourceGraph && Schema);

	const FEdGraphPinType& PinType{PayloadPin->PinType};

	if (PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
	{
		if (PayloadPin->LinkedTo.Num() == 0)
		{
			// If no payload type is specified and we're not trying to connect the output to anything ignore the rest of this step
			return true;
		}
		else
		{
			return false;
		}
	}

	UK2Node_TemporaryVariable* TempVarOutput
	{
		CompilerContext.SpawnInternalVariable
		(
			this, PinType.PinCategory, PinType.PinSubCategory,
			PinType.PinSubCategoryObject.Get(), PinType.ContainerType, PinType.PinValueType
		)
	};

	UK2Node_CallFunction* const CallGetPayloadNode{CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph)};
	CallGetPayloadNode->FunctionReference.SetExternalMember(L"GetPayload", CurrentProperty->GetOwnerClass());
	CallGetPayloadNode->AllocateDefaultPins();

	// Hook up the self connection
	if (UEdGraphPin* GetPayloadCallSelfPin{Schema->FindSelfPin(*CallGetPayloadNode, EGPD_Input)})
	{
		bIsErrorFree &= Schema->TryCreateConnection(GetPayloadCallSelfPin, ProxyObjectVar.TempVar->GetVariablePin());

		// Hook the activate node up in the exec chain
		UEdGraphPin* GetPayloadExecPin{CallGetPayloadNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute)};
		UEdGraphPin* GetPayloadThenPin{CallGetPayloadNode->FindPinChecked(UEdGraphSchema_K2::PN_Then)};

		// UEdGraphPin* LastThenPin{nullptr};
		UEdGraphPin* GetPayloadPin{CallGetPayloadNode->FindPinChecked(L"OutPayload")};
		bIsErrorFree &= Schema->TryCreateConnection(TempVarOutput->GetVariablePin(), GetPayloadPin);


		UK2Node_AssignmentStatement* AssignNode{CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph)};
		AssignNode->AllocateDefaultPins();
		bIsErrorFree &= Schema->TryCreateConnection(GetPayloadThenPin, AssignNode->GetExecPin());
		bIsErrorFree &= Schema->TryCreateConnection(PayloadVar.TempVar->GetVariablePin(), AssignNode->GetVariablePin());
		AssignNode->NotifyPinConnectionListChanged(AssignNode->GetVariablePin());
		bIsErrorFree &= Schema->TryCreateConnection(AssignNode->GetValuePin(), TempVarOutput->GetVariablePin());
		AssignNode->NotifyPinConnectionListChanged(AssignNode->GetValuePin());


		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*InOutLastActivatedThenPin, *AssignNode->GetThenPin()).CanSafeConnect();
		bIsErrorFree &= Schema->TryCreateConnection(InOutLastActivatedThenPin, GetPayloadExecPin);

		// Hook up the actual channel connection
		UEdGraphPin* OutActualChannelPin{GetOutputChannelPin()};
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*OutActualChannelPin, *ActualChannelVar.TempVar->GetVariablePin()).CanSafeConnect();
	}

	return bIsErrorFree;
}

void UG2Node_AsyncAction_ListenForGameplayMsgs::RefreshOutputPayloadType()
{
	UEdGraphPin* PayloadPin{GetPayloadPin()};

	if (const UEdGraphPin* PayloadTypePin{GetPayloadTypePin()}; PayloadTypePin->DefaultObject != PayloadPin->PinType.PinSubCategoryObject)
	{
		if (PayloadPin->SubPins.Num() > 0)
		{
			GetSchema()->RecombinePin(PayloadPin);
		}

		PayloadPin->PinType.PinSubCategoryObject = PayloadTypePin->DefaultObject;
		PayloadPin->PinType.PinCategory = !PayloadTypePin->DefaultObject ? UEdGraphSchema_K2::PC_Wildcard : UEdGraphSchema_K2::PC_Struct;
	}
}

UEdGraphPin* UG2Node_AsyncAction_ListenForGameplayMsgs::GetPayloadPin() const
{
	UEdGraphPin* Pin{FindPinChecked(UG2Node_AsyncAction_ListenForGameplayMsgsHelper::PayloadPinName)};
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UG2Node_AsyncAction_ListenForGameplayMsgs::GetPayloadTypePin() const
{
	UEdGraphPin* Pin{FindPinChecked(UG2Node_AsyncAction_ListenForGameplayMsgsHelper::PayloadTypePinName)};
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UG2Node_AsyncAction_ListenForGameplayMsgs::GetOutputChannelPin() const
{
	UEdGraphPin* Pin{FindPinChecked(UG2Node_AsyncAction_ListenForGameplayMsgsHelper::ActualChannelPinName)};
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
