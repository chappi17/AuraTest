// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Inventory/GmRIS_StorageComponent.h"
#include "Components/ActorComponent.h"

#include "GmRIS_StorageManagerComponent.generated.h"

class UGmRIS_InventoryItemInstance;
class UGmRIS_InventoryItemDefinition;

USTRUCT(BlueprintType)
struct FGmUniqueStorageItemInfo
{
	GENERATED_BODY()

	FGmUniqueStorageItemInfo()
		:
	TargetItemDef(nullptr),
	bNeedAll(false),
	UniqueStorageID()
	{}
	
	FGmUniqueStorageItemInfo(const TSubclassOf<UGmRIS_InventoryItemDefinition> InTargetItemDef, const bool InNeedAll, const uint32 InUniqueInstID)
	:
	TargetItemDef(InTargetItemDef),
	bNeedAll(InNeedAll),
	UniqueStorageID(InUniqueInstID)
	{}

	FORCEINLINE bool CanApplyInteraction(const uint32 InUniqueIDForComparison) const
	{
		return UniqueStorageID != 0 && InUniqueIDForComparison == UniqueStorageID;
	}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Request Info for Storage Item")
	TSubclassOf<UGmRIS_InventoryItemDefinition> TargetItemDef;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Request Info for Storage Item")
	bool bNeedAll;

private:
	
	uint32 UniqueStorageID;
	
};

/**
 * Manages an storage.
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_StorageManagerComponent : public UGmRIS_StorageComponent
{
	GENERATED_BODY()

public:
	
	UGmRIS_StorageManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~ UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent Interface

	// UFUNCTION(BlueprintCallable, Category = "GmRIS|Storage")
	void AddInitialStorageItem(TArray<FGmUniqueStorageItemInfo> InInitialStorageItemDefs);

	UFUNCTION(BlueprintCallable, Category = "GmRIS|Storage")
	void AddItemToStorage(TSubclassOf<UGmRIS_InventoryItemDefinition> InTargetItemDef, const int32 InItemCount = 1);

	UFUNCTION(BlueprintCallable, Category = "GmRIS|Storage")
	void RemoveItemToStorage(TSubclassOf<UGmRIS_InventoryItemDefinition> InTargetItemDef, const bool InIsAll);

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings")
	TArray<FGmUniqueStorageItemInfo> InitStorage;

private:
	
	UFUNCTION()
	void UpdateSpecificStorageItems(FGameplayTag InTargetTag, const FGmUniqueStorageItemInfo& InNewItemInfo);
};
