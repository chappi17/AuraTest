// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Interaction/GmRIS_InteractionStatics.h"

#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/HitResult.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"
#include "Interaction/IGmRIS_InteractableTarget.h"
#include "Misc/AssertionMacros.h"
#include "Templates/Casts.h"
#include "UObject/Object.h"

UGmRIS_InteractionStatics::UGmRIS_InteractionStatics()
	: Super(FObjectInitializer::Get())
{}

AActor* UGmRIS_InteractionStatics::GetActorFromInteractableTarget(const TScriptInterface<IGmRIS_InteractableTarget> InteractableTarget)
{
	if (UObject* Object{InteractableTarget.GetObject()})
	{
		if (AActor* Actor{Cast<AActor>(Object)})
		{
			return Actor;
		}
		else if (const UActorComponent* ActorComponent{Cast<UActorComponent>(Object)})
		{
			return ActorComponent->GetOwner();
		}
		else
		{
			unimplemented();
		}
	}

	return nullptr;
}

void UGmRIS_InteractionStatics::GetInteractableTargetsFromActor(AActor* Actor, TArray<TScriptInterface<IGmRIS_InteractableTarget>>& OutInteractableTargets)
{
	// If the actor is directly interactable, return that.
	if (const TScriptInterface<IGmRIS_InteractableTarget> InteractableActor(Actor); InteractableActor)
	{
		OutInteractableTargets.Add(InteractableActor);
	}

	// If the actor isn't interactable, it might have a component that has a interactable interface.
	TArray<UActorComponent*> InteractableComponents = Actor ? Actor->GetComponentsByInterface(UGmRIS_InteractableTarget::StaticClass()) : TArray<UActorComponent*>();
	for (UActorComponent* InteractableComponent : InteractableComponents)
	{
		OutInteractableTargets.Add(TScriptInterface<IGmRIS_InteractableTarget>(InteractableComponent));
	}
}

void UGmRIS_InteractionStatics::AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IGmRIS_InteractableTarget>>& OutInteractableTargets)
{
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		TScriptInterface<IGmRIS_InteractableTarget> InteractableActor(Overlap.GetActor());
		if (InteractableActor)
		{
			OutInteractableTargets.AddUnique(InteractableActor);
		}

		TScriptInterface<IGmRIS_InteractableTarget> InteractableComponent(Overlap.GetComponent());
		if (InteractableComponent)
		{
			OutInteractableTargets.AddUnique(InteractableComponent);
		}
	}
}

void UGmRIS_InteractionStatics::AppendInteractableTargetsFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<IGmRIS_InteractableTarget>>& OutInteractableTargets)
{
	if (const TScriptInterface<IGmRIS_InteractableTarget> InteractableActor(HitResult.GetActor()); InteractableActor)
	{
		OutInteractableTargets.AddUnique(InteractableActor);
	}

	if (const TScriptInterface<IGmRIS_InteractableTarget> InteractableComponent(HitResult.GetComponent()); InteractableComponent)
	{
		OutInteractableTargets.AddUnique(InteractableComponent);
	}
}

bool UGmRIS_InteractionStatics::GmTriggerInteraction(const UGameplayAbility* InOwningAbility,
	const FGameplayTag InEventTag, const FGmRIS_InteractionOpt& InTargetOpt)
{
	return GmTriggerInteraction_Internal(InOwningAbility, InEventTag, InTargetOpt);
}

bool UGmRIS_InteractionStatics::GmTriggerInteraction_Internal(const UGameplayAbility* InOwningAbility, const FGameplayTag& InEventTag, const FGmRIS_InteractionOpt& InTargetOpt)
{
	if (!InOwningAbility || !InEventTag.IsValid())
	{
		return false;
	}
	if (InOwningAbility->GetAbilitySystemComponentFromActorInfo())
	{
		AActor* InteractableTargetActor{GetActorFromInteractableTarget(InTargetOpt.InteractableTarget)};
		
		FGameplayEventData Loc_NewPayload;
		Loc_NewPayload.EventTag = InEventTag;
		Loc_NewPayload.Instigator = InOwningAbility->GetAvatarActorFromActorInfo();
		Loc_NewPayload.Target = InteractableTargetActor;

		// If needed we allow the interactable target to manipulate the event data so that for example, a button on the wall
		// may want to specify a door actor to execute the ability on, so it might choose to override Target to be the
		// Like Gm Interactable Prop System::GmA_IPsDoor actor.
		InTargetOpt.InteractableTarget->CustomizeInteractionEventData(InEventTag, Loc_NewPayload);

		// Grab the target actor off the payload we're going to use it as the 'avatar' for the interaction, and the
		// source InteractableTarget actor as the owner actor.
		AActor* TargetActor{const_cast<AActor*>(ToRawPtr(Loc_NewPayload.Target))};

		// The actor info needed for the interaction.
		FGameplayAbilityActorInfo ActorInfo;
		ActorInfo.InitFromActor(InteractableTargetActor, TargetActor, InTargetOpt.TargetAbilitySystem);

		// Trigger the ability using event tag.
		return InTargetOpt.TargetAbilitySystem->TriggerAbilityFromGameplayEvent
		(
			InTargetOpt.TargetInteractionAbilityHandle,
			&ActorInfo,
			InEventTag,
			&Loc_NewPayload,
			*InTargetOpt.TargetAbilitySystem
		);
	}
	return false;
}

bool UGmRIS_InteractionStatics::OnInteractionInputPressed(const UGameplayAbility* InOwningAbility,
	const FGameplayTag InEventTag, TArray<FGmRIS_InteractionOpt> InTargetOpts)
{
	if (!InTargetOpts.IsEmpty())
	{
		return GmTriggerInteraction_Internal(InOwningAbility, InEventTag, InTargetOpts[0]);
	}
	return false;
}
