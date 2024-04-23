// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GmAT_ToggleWidgetHandler.generated.h"

class UGmRISW_InventoryContainer;

USTRUCT(BlueprintType)
struct FGmCreatedInventoryWidgetInfo
{
	GENERATED_BODY()

public:

	FGmCreatedInventoryWidgetInfo()
		:
	CreatedInventoryWidget(nullptr)
	{}

	FGmCreatedInventoryWidgetInfo(UGmRISW_InventoryContainer* InCreatedWidget)
		:
	CreatedInventoryWidget(InCreatedWidget)
	{}
	
	UPROPERTY(BlueprintReadOnly, Category = "Created Inventory Widget", DisplayName = "Inventory widget immediately after becoming visible.")
	TObjectPtr<UGmRISW_InventoryContainer> CreatedInventoryWidget;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGmRIS_CreatedInventoryWidget, const FGmCreatedInventoryWidgetInfo&, OutInventoryWidgetImmediatelyAfterBecomingVisible);

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmAT_ToggleWidgetHandler : public UAbilityTask
{
	GENERATED_BODY()
	
public:

	UGmAT_ToggleWidgetHandler(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintAssignable, DisplayName = "Immediately after the inventory widget appears on the screen")
	FGmRIS_CreatedInventoryWidget AfterCreatedInventoryWidget;

	UPROPERTY(BlueprintAssignable, DisplayName = "Immediately after the inventory widget is removed")
	FGmRIS_CreatedInventoryWidget RemovedInventoryWidget;

	//~UAbilityTask Interface
	virtual void Activate() override;
	//~End of UAbilityTask Interface

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "InOwningAbility", DefaultToSelf = "InOwningAbility", BlueprintInternalUseOnly = "TRUE"), DisplayName = "Toggle Inventory Widget Handler")
	static UGmAT_ToggleWidgetHandler* ToggleWidgetHandler(UGameplayAbility* InOwningAbility, UPARAM(DisplayName = "Inventory Widget Class Reference") TSubclassOf<UGmRISW_InventoryContainer> InInventoryWidgetClassRef);

private:

	//~UAbilityTask Interface
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~End of UAbilityTask Interface

	void GmUpdateOrToggleWidget() const;

	UPROPERTY()
	TSubclassOf<UGmRISW_InventoryContainer> InventoryContainerWidgetClassRef{nullptr};
	
};
