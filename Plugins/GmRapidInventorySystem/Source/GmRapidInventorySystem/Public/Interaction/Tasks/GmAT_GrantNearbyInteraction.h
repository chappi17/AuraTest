// Copyright Dev.GaeMyo 2024. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Containers/Map.h"
#include "Engine/EngineTypes.h"
#include "UObject/UObjectGlobals.h"

#include "GmAT_GrantNearbyInteraction.generated.h"

class UGameplayAbility;
class UObject;
struct FFrame;
struct FGameplayAbilitySpecHandle;
struct FObjectKey;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmAT_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()

public:

	UGmAT_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UAbilityTask Interface
	virtual void Activate() override;
	//~End of UAbilityTask Interface

	/** Wait until an overlap occurs. This will need to be better fleshed out so we can specify game specific collision requirements */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "InOwningAbility", DefaultToSelf = "InOwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGmAT_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(UGameplayAbility* InOwningAbility, const float InteractionScanRange, const float InteractionScanRate);

private:

	//~UAbilityTask Interface
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~End of UAbilityTask Interface
	
	void QueryInteractables();

	float InteractionScanRange{100};
	float InteractionScanRate{0.100};

	FTimerHandle QueryTimerHandle;

	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
