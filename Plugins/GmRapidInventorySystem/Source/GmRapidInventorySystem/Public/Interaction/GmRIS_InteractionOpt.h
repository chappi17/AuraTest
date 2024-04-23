// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GmRIS_InteractionOpt.generated.h"

class IGmRIS_InteractableTarget;
class UUserWidget;

USTRUCT(BlueprintType)
struct FGmRIS_InteractionOpt
{
	GENERATED_BODY()
	
	/** The interactable target */
	UPROPERTY(BlueprintReadWrite, Category = InteractionOption)
	TScriptInterface<IGmRIS_InteractableTarget> InteractableTarget;

	// /** Simple text the interaction might return */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionOption)
	// FText Text;
	//
	// /** Simple sub-text the interaction might return */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionOption)
	// FText SubText;

	// METHODS OF INTERACTION

	// 1) Place an ability on the avatar that they can activate when they perform interaction.

	/** The ability to grant the avatar when they get near interactable objects. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InteractionOption)
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	// - OR -

	// 2) Allow the object we're interacting with to have its own ability system and interaction ability, that we can activate instead.

	/** The ability system on the target that can be used for the TargetInteractionHandle and sending the event, if needed. */
	UPROPERTY(BlueprintReadOnly, Category = InteractionOption)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystem{nullptr};

	/** The ability spec to activate on the object for this option. */
	UPROPERTY(BlueprintReadOnly, Category = InteractionOption)
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;

	// UI
	
	/** The widget to show for this kind of interaction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionOption)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;


public:
	
	FORCEINLINE bool operator==(const FGmRIS_InteractionOpt& Other) const
	{
		return InteractableTarget == Other.InteractableTarget &&
			InteractionAbilityToGrant == Other.InteractionAbilityToGrant&&
			TargetAbilitySystem == Other.TargetAbilitySystem &&
			TargetInteractionAbilityHandle == Other.TargetInteractionAbilityHandle &&
			InteractionWidgetClass == Other.InteractionWidgetClass/* &&
			Text.IdenticalTo(Other.Text) &&
			SubText.IdenticalTo(Other.SubText)*/;
	}

	FORCEINLINE bool operator!=(const FGmRIS_InteractionOpt& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FGmRIS_InteractionOpt& Other) const
	{
		return InteractableTarget.GetInterface() < Other.InteractableTarget.GetInterface();
	}
};
