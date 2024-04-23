// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Interaction/Tasks/GmAT_GrantNearbyInteraction.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Containers/Array.h"
#include "Delegates/Delegate.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameplayAbilitySpec.h"
#include "Interaction/IGmRIS_InteractableTarget.h"
#include "Interaction/GmRIS_InteractionOpt.h"
#include "Interaction/GmRIS_InteractionQuery.h"
#include "Interaction/GmRIS_InteractionStatics.h"
#include "Math/Quat.h"
#include "Stats/Stats2.h"
#include "Templates/Casts.h"
#include "Templates/SubclassOf.h"
#include "TimerManager.h"
#include "Interaction/GmRIS_InteractionQuery.h"
#include "Interaction/GmRIS_InteractionStatics.h"
#include "Interaction/IGmRIS_InteractableTarget.h"
#include "Physics/GmCollisionChannels.h"
#include "UObject/ObjectKey.h"
#include "UObject/ScriptInterface.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

UGmAT_GrantNearbyInteraction::UGmAT_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

UGmAT_GrantNearbyInteraction* UGmAT_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(
	UGameplayAbility* InOwningAbility, const float InteractionScanRange, const float InteractionScanRate)
{
	UGmAT_GrantNearbyInteraction* Loc_NewObj{NewAbilityTask<UGmAT_GrantNearbyInteraction>(InOwningAbility)};
	Loc_NewObj->InteractionScanRange = InteractionScanRange;
	Loc_NewObj->InteractionScanRate = InteractionScanRate;
	return Loc_NewObj;
}

void UGmAT_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();

	GetWorld()->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

void UGmAT_GrantNearbyInteraction::OnDestroy(const bool bInOwnerFinished)
{
	if (const UWorld* World{GetWorld()})
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGmAT_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* CurWorld{GetWorld()};
	if (AActor* ActorOwner{GetAvatarActor()}; CurWorld && ActorOwner)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(UGmAT_GrantNearbyInteraction), false);
		TArray<FOverlapResult> OverlapResults;
		
		CurWorld->OverlapMultiByChannel(
			OUT OverlapResults, ActorOwner->GetActorLocation(), FQuat::Identity,
			Gm_TraceChannel_Interaction, FCollisionShape::MakeSphere(InteractionScanRange), Params);
		
		if(OverlapResults.IsEmpty()) return;
		
		TArray<TScriptInterface<IGmRIS_InteractableTarget>> InteractableTargets;
		UGmRIS_InteractionStatics::AppendInteractableTargetsFromOverlapResults(OverlapResults,
			OUT InteractableTargets);
			
		FGmRIS_InteractionQuery GmInteractionQuery(ActorOwner,
			Cast<AController>(ActorOwner->GetOwner()));

		TArray<FGmRIS_InteractionOpt> GmOptions;
		for (TScriptInterface<IGmRIS_InteractableTarget>& InteractiveTarget : InteractableTargets)
		{
			FGmRIS_InteractionOptBuilder InteractionBuilder(InteractiveTarget, GmOptions);
			InteractiveTarget->GatherInteractionOptions(GmInteractionQuery, InteractionBuilder);
		}

		// Check if any of the options need to grant the ability to the user before they can be used.
		for (auto& [InteractableTarget, /*Text, SubText, */InteractionAbilityToGrant, TargetAbilitySystem, TargetInteractionAbilityHandle, InteractionWidgetClass] : GmOptions)
		{
			if (InteractionAbilityToGrant)
			{
				// Grant the ability to the GAS, otherwise it won't be able to do whatever the interaction is.
				if (FObjectKey Loc_ObjKey(InteractionAbilityToGrant); !InteractionAbilityCache.Find(Loc_ObjKey))
				{
					FGameplayAbilitySpec Spec(InteractionAbilityToGrant, 1, INDEX_NONE, this);
					FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
					InteractionAbilityCache.Add(Loc_ObjKey, Handle);
				}
			}
		}
	}
}
