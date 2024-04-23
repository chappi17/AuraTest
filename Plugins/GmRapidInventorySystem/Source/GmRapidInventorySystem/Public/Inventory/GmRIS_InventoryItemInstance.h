// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HAL/Platform.h"
#include "Core/GmGameplayTagStack.h"
#include "GameFramework/PlayerController.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "UObject/UObjectGlobals.h"

#include "GmRIS_InventoryItemInstance.generated.h"

class UGmRIS_InventoryItemFragment;
class UGmRIS_InventoryItemDefinition;
class UGmRIS_InventoryItemInstance;
struct FFrame;
struct FGameplayTag;

USTRUCT(BlueprintType)
struct FGmSimpleItemInstance
{
	GENERATED_BODY()

	FGmSimpleItemInstance()
		:
	ItemInst(nullptr),
	OwningActor(nullptr),
	bIsAllQuantities(false),
	bIsDrop(false)
	{}

	FGmSimpleItemInstance(UGmRIS_InventoryItemInstance* InTargetItemInst, const AActor* InOwningActor)
		:
	ItemInst(InTargetItemInst),
	OwningActor(InOwningActor),
	bIsAllQuantities(false),
	bIsDrop(false)
	{}

	FGmSimpleItemInstance(UGmRIS_InventoryItemInstance* InTargetItemInst, const AActor* InOwningActor,
		const bool InIsAllQuantities, const bool InIsDrop = false)
		:
	ItemInst(InTargetItemInst),
	OwningActor(InOwningActor),
	bIsAllQuantities(InIsAllQuantities),
	bIsDrop(InIsDrop)
	{}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simple Item Instance")
	TObjectPtr<UGmRIS_InventoryItemInstance> ItemInst;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simple Item Instance")
	TObjectPtr<const AActor> OwningActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simple Item Instance")
	bool bIsAllQuantities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simple Item Instance")
	bool bIsDrop;


};

USTRUCT(BlueprintType)
struct FGmConstItemInst
{
	GENERATED_BODY()

	FGmConstItemInst()
		:
	ConstItemInst(nullptr)
	{}

	explicit FGmConstItemInst(UGmRIS_InventoryItemInstance* InItemInst,
		UObject* InOwningPC)
		:
	ConstItemInst(InItemInst),
	OwningPC(Cast<APlayerController>(InOwningPC))
	{}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Const Item Inst")
	TObjectPtr<UGmRIS_InventoryItemInstance> ConstItemInst;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Const Item Inst")
	TObjectPtr<APlayerController> OwningPC;
	
};

UENUM(BlueprintType)
enum class EGmItemType : uint8
{
	Misc,
	Consumable,
	Equipable
};

UCLASS(BlueprintType)
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_InventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:

	UGmRIS_InventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual FORCEINLINE bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddStatTagStack(FGameplayTag InTag, int32 InStackCount, const bool InIsInitTagSetting = false);
	
	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveStatTagStack(FGameplayTag InTag, int32 InStackCount);
	
	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetStatTagStackCount(FGameplayTag InTag) const;
	
	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasStatTag(FGameplayTag InTag) const;
	
	FORCEINLINE TSubclassOf<UGmRIS_InventoryItemDefinition> GetItemDefinition() const
	{
		return ItemDef;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = InFragmentClassRef), Category = GmRIS)
	const UGmRIS_InventoryItemFragment* FindFragmentByClass(TSubclassOf<UGmRIS_InventoryItemFragment> InFragmentClassRef) const;

	template <typename RstC>
	const RstC* FindFragmentByClass() const
	{
		return (RstC*)FindFragmentByClass(RstC::StaticClass());
	}

private:

#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& OutContext, UE::Net::EFragmentRegistrationFlags InRegistrationFlags) override;
#endif // UE_WITH_IRIS

	friend struct FGmRISInventoryList;
	void SetItemDefinition(TSubclassOf<UGmRIS_InventoryItemDefinition> InDefinition);
	
	UPROPERTY(Replicated)
	FGmGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(Replicated)
	TSubclassOf<UGmRIS_InventoryItemDefinition> ItemDef;
	
};
