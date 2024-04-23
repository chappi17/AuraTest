// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"
#include "Interaction/GmRIS_InteractionOpt.h"
#include "IGmRIS_InteractableTarget.generated.h"

struct FGmRIS_InteractionQuery;

class FGmRIS_InteractionOptBuilder
{
public:
	
	FGmRIS_InteractionOptBuilder(const TScriptInterface<IGmRIS_InteractableTarget> InterfaceTargetScope, TArray<FGmRIS_InteractionOpt>& InteractOptions)
		:
	Scope(InterfaceTargetScope),
	Options(InteractOptions)
	{}

	void AddInteractionOption(const FGmRIS_InteractionOpt& Option) const
	{
		auto& [InteractableTarget, /*Text, SubText, */InteractionAbilityToGrant, TargetAbilitySystem, TargetInteractionAbilityHandle, InteractionWidgetClass]{Options.Add_GetRef(Option)};
		InteractableTarget = Scope;
	}

private:
	
	TScriptInterface<IGmRIS_InteractableTarget> Scope;
	TArray<FGmRIS_InteractionOpt>& Options;
	
};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGmRIS_InteractableTarget : public UInterface
{
	GENERATED_BODY()
};

class IGmRIS_InteractableTarget
{
	GENERATED_BODY()

public:
	
	virtual void GatherInteractionOptions(const FGmRIS_InteractionQuery& InteractQuery, FGmRIS_InteractionOptBuilder& InInteractionBuilder) = 0;

	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) { }
	
};
