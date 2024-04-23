// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GmRIS_InventoryItemInstance.h"
#include "GmRIS_StorageComponent.h"
// #include "GameFramework/GameplayMsgSubsystem.h"

#include "GmRIS_InventoryManagerComponent.generated.h"

class UGmRISW_MainLayout;
class UGmRISW_InventoryContainer;
class UGmRIS_InventoryItemDefinition;
class UGmRIS_InventoryItemInstance;
class UObject;
class UGmRIS_InventoryManagerComponent;
struct FGmRISInventoryList;
struct FFrame;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;


/**
 * Manages an inventory.
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_InventoryManagerComponent : public UGmRIS_StorageComponent
{
	GENERATED_BODY()

public:
	
	UGmRIS_InventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ UActorComponent Interface.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent Interface.

	// Init settings.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn), Category = "Init Settings")
	TSubclassOf<UGmRISW_MainLayout> MainLayoutWidgetClassRef{nullptr};

	// Utils
	bool ConsumeItemsByDefinition(const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef, const int32 InNumToConsume);
	
	void GmSetInventoryMainWidget(UGmRISW_InventoryContainer* InNewInventoryWidgetRef);
	
	FORCEINLINE UGmRISW_InventoryContainer* GmGetInventoryMainWidget() const
	{
		return GmMainInventoryWidgetCache;
	}

	UFUNCTION(Client, Reliable)
	void ItemHasBeenUsed(FGameplayTag InTargetTag, const FGmSimpleItemInstance& InUsedItemInst);

private:

	UPROPERTY()
	TObjectPtr<UGmRISW_MainLayout> GmMainLayoutWidgetCache{nullptr};

	UPROPERTY()
	TObjectPtr<UGmRISW_InventoryContainer> GmMainInventoryWidgetCache{nullptr};
	
	UFUNCTION(Server, Reliable)
	void Server_ItemHasBeenUsed(const FGmSimpleItemInstance& InUsedItemInst);

	void Internal_ItemHasBeenUsed(const FGmSimpleItemInstance& InUsedItemInst);

	// Client, Unreliable
	virtual void Internal_ClientNotifyAddedEvent(const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InAddedCount) const override;
	virtual void Internal_ClientNotifyRemovedEvent(const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InRemovedCount) const override;

};
