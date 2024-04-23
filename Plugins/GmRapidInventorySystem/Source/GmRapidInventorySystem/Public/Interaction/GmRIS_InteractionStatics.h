// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Containers/Array.h"
#include "Engine/OverlapResult.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ScriptInterface.h"
#include "UObject/UObjectGlobals.h"

#include "GmRIS_InteractionStatics.generated.h"

struct FGmRIS_InteractionOpt;
class UGameplayAbility;
struct FGameplayTag;
class AActor;
class IGmRIS_InteractableTarget;
class UObject;
struct FFrame;
struct FHitResult;
struct FOverlapResult;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_InteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UGmRIS_InteractionStatics();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static AActor* GetActorFromInteractableTarget(const TScriptInterface<IGmRIS_InteractableTarget> InteractableTarget);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static void GetInteractableTargetsFromActor(AActor* Actor, TArray<TScriptInterface<IGmRIS_InteractableTarget>>& OutInteractableTargets);

	static void AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IGmRIS_InteractableTarget>>& OutInteractableTargets);
	static void AppendInteractableTargetsFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<IGmRIS_InteractableTarget>>& OutInteractableTargets);
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "InOwningAbility", DefaultToSelf = "InOwningAbility"/*, BlueprintInternalUseOnly = "TRUE"*/)) // When you enter the "BlueprintInternalUseOnly" option, it is not displayed in the editor.
	static bool GmTriggerInteraction(const UGameplayAbility* InOwningAbility, const FGameplayTag InEventTag, const FGmRIS_InteractionOpt& InTargetOpt);
	
	static bool GmTriggerInteraction_Internal(const UGameplayAbility* InOwningAbility, const FGameplayTag& InEventTag, const FGmRIS_InteractionOpt& InTargetOpt);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "InOwningAbility", DefaultToSelf = "InOwningAbility"/*, BlueprintInternalUseOnly = "TRUE"*/)) // When you enter the "BlueprintInternalUseOnly" option, it is not displayed in the editor.
	static bool OnInteractionInputPressed(const UGameplayAbility* InOwningAbility, const FGameplayTag InEventTag, TArray<FGmRIS_InteractionOpt> InTargetOpts);
	
};
