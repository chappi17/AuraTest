// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Interaction/GmRIS_InteractionQuery.h"
#include "Interaction/Tasks/GmAT_WaitForInteractableTargets.h"

#include "GmAT_WFITs_SingleLineTrace.generated.h"

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmAT_WFITs_SingleLineTrace : public UGmAT_WaitForInteractableTargets
{
	GENERATED_BODY()

public:

	UGmAT_WFITs_SingleLineTrace(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
   
	//~UAbilityTask Interface
	virtual void Activate() override;
	//~End of UAbilityTask Interface

	/** Wait until we trace new set of interactables.  This task automatically loops. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"), DisplayName = "Wait for Targets Single Line Trace")
	static UGmAT_WFITs_SingleLineTrace* WFITs_SingleLineTrace(UGameplayAbility* OwningAbility, const FGmRIS_InteractionQuery InInteractionQuery, const FCollisionProfileName TraceProfile, const FGameplayAbilityTargetingLocationInfo StartLocation, const float InteractionScanRange = 100.f, const float InteractionScanRate = 0.1f, UPARAM(DisplayName = "Show Debug?") const bool InShowDebug = false, UPARAM(DisplayName = "Is TopDown?")const bool InIsTopDown = false);

private:

	//~UAbilityTask Interface
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~End of UAbilityTask Interface
   
	void ActivateTrace();

	FGmRIS_InteractionQuery InteractionQuery;
	FGameplayAbilityTargetingLocationInfo StartLocation;
	
	float InteractionScanRate{0.100};

	FTimerHandle GmActiveTH;
   
};
