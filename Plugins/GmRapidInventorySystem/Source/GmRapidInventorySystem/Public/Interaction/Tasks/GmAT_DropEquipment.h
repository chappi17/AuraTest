// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/Tasks/AbilityTask.h"

#include "GmAT_DropEquipment.generated.h"

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmAT_DropEquipment : public UAbilityTask
{
	GENERATED_BODY()

public:

	UGmAT_DropEquipment(const FObjectInitializer& ObjectInitializer);

	//~UAbilityTask Interface
	virtual void Activate() override;
	//~End of UAbilityTask Interface

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "InOwningAbility", DefaultToSelf = "InOwningAbility", BlueprintInternalUseOnly = "TRUE", Keywords = "Drop"), DisplayName = "Gm Drop Equipment")
	static UGmAT_DropEquipment* DropEquipment(UGameplayAbility* InOwningAbility);

private:

	void GmDropEquipment() const;
	
};
