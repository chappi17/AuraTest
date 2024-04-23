// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/ControllerComponent.h"

#include "GmRIS_QuickBarComponent.generated.h"

struct FGmSimpleItemInstance;
struct FGameplayTag;
class UGmRIS_EquipmentInstance;
class UGmRIS_EquipmentManagerComponent;
class AActor;
class UGmRIS_InventoryItemInstance;

USTRUCT(BlueprintType)
struct FGmRISQuickBarSlotsChangedMsg
{
	GENERATED_BODY()

public:

	FGmRISQuickBarSlotsChangedMsg()
		:
	Owner(nullptr)
	{}

	FGmRISQuickBarSlotsChangedMsg(AActor* InOwner, TArray<TObjectPtr<UGmRIS_InventoryItemInstance>> InSlots)
		:
	Owner(InOwner),
	Slots(InSlots)
	{}

	UPROPERTY(BlueprintReadOnly, Category = GmRISInventory)
	TObjectPtr<AActor> Owner;

	UPROPERTY(BlueprintReadOnly, Category = GmRISInventory)
	TArray<TObjectPtr<UGmRIS_InventoryItemInstance>> Slots;
};

USTRUCT(BlueprintType)
struct FGmRISQuickBarActiveIndexChangedMsg
{
	GENERATED_BODY()

public:

	FGmRISQuickBarActiveIndexChangedMsg()
		:
	Owner(nullptr),
	ActiveIndex(0)
	{}

	FGmRISQuickBarActiveIndexChangedMsg(AActor* InOwner, const int32 InActiveIndex)
		:
	Owner(InOwner),
	ActiveIndex(InActiveIndex)
	{}

	UPROPERTY(BlueprintReadOnly, Category = GmRISInventory)
	TObjectPtr<AActor> Owner;

	UPROPERTY(BlueprintReadOnly, Category = GmRISInventory)
	int32 ActiveIndex;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_QuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:

	UGmRIS_QuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings|Debug", meta = (ExposeOnSpawn = true))
	bool bIsDebug{false};

	//~AActor Interface
	virtual void BeginPlay() override;
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~End of AActor Interface

	UFUNCTION(BlueprintCallable, Category = GmRIS)
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category = GmRIS)
	void CycleActiveSlotBackward();
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = GmRISInventory)
	void SetActiveSlotIndex(const int32 InNewIndex);

	UFUNCTION()
	void QuickSlotItemSynchronization(FGameplayTag InTargetTag, const FGmSimpleItemInstance& InTargetItemInst);

	UFUNCTION(Server, Reliable)
	void Server_QuickSlotItemSynchronization(const FGmSimpleItemInstance& InTargetItemInst);

	void Internal_QuickSlotItemSynchronization(const FGmSimpleItemInstance& InTargetItemInst);
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = GmRISInventory)
	TArray<UGmRIS_InventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = GmRISInventory)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = GmRISInventory)
	UGmRIS_InventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = GmRISInventory)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GmRISInventory)
	void AddItemToSlot(const int32 InSlotIndex, UGmRIS_InventoryItemInstance* InItem);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GmRISInventory)
	UGmRIS_InventoryItemInstance* RemoveItemFromSlot(const int32 InSlotIndex);
	
protected:

	UPROPERTY()
	int32 NumSlots{5};
	
	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();
	
private:
	
	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<TObjectPtr<UGmRIS_InventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex{-1};

	UPROPERTY()
	TObjectPtr<UGmRIS_EquipmentInstance> EquippedItem;

	UGmRIS_EquipmentManagerComponent* FindEquipmentManager() const;

	void UnEquipItemInSlot();
	
	void EquipOrConsumeItemInSlot();

	UFUNCTION(Client, Reliable)
	void GmBroadcastUsingItem(UGmRIS_InventoryItemInstance* InSlotItem) const;

	// float CurrentDeltaTime;
	bool FirstSelectConsumableItem{false};
	
};
