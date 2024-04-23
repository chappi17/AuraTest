// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "HAL/Platform.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "UObject/Interface.h"
#include "UObject/ScriptInterface.h"
#include "UObject/UObjectGlobals.h"

#include "GmRISI_Pickable.generated.h"

class UGmRIS_InventoryManagerComponent;
class UGmRIS_InventoryItemInstance;
class AActor;
class UGmRIS_InventoryItemDefinition;
struct FFrame;

USTRUCT(BlueprintType)
struct GMRAPIDINVENTORYSYSTEM_API FGmRIS_PickupTemplate
{
	GENERATED_BODY()

	FGmRIS_PickupTemplate()
		:
	ItemDef(nullptr)
	{}

	FGmRIS_PickupTemplate(const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef)
		:
	ItemDef(InItemDef)
	{}
	
	// UPROPERTY(EditAnywhere, Category = PickupTemplate)
	// int32 StackCount{1};

	UPROPERTY(EditAnywhere, Category = PickupTemplate)
	TSubclassOf<UGmRIS_InventoryItemDefinition> ItemDef;
	
};

USTRUCT(BlueprintType)
struct GMRAPIDINVENTORYSYSTEM_API FGmRIS_PickupInstance
{
	GENERATED_BODY()

	FGmRIS_PickupInstance()
		:
	Item(nullptr)
	{}

	FGmRIS_PickupInstance(UGmRIS_InventoryItemInstance* InItemInst)
		:
	Item(InItemInst)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PickupInstance)
	TObjectPtr<UGmRIS_InventoryItemInstance> Item;
};

USTRUCT(BlueprintType)
struct GMRAPIDINVENTORYSYSTEM_API FGmRIS_InventoryPickup
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InventoryPickup)
	TArray<FGmRIS_PickupInstance> Instances;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InventoryPickup)
	TArray<FGmRIS_PickupTemplate> Templates;
};

UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UGmRISI_Pickable : public UInterface
{
	GENERATED_BODY()
};

class IGmRISI_Pickable
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = Interface)
	virtual FGmRIS_InventoryPickup GetPickupInventory() const = 0;
};

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRISI_PickableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UGmRISI_PickableStatics();
	
	UFUNCTION(BlueprintPure, Category = GmRISGlobal)
	static TScriptInterface<IGmRISI_Pickable> GetFirstPickupAbleFromActor(AActor* InActor);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "Ability"), Category = GmRISGlobal)
	static void AddPickupToInventory(UGmRIS_InventoryManagerComponent* InInventoryComponent, const TScriptInterface<IGmRISI_Pickable> InPickup);
};
