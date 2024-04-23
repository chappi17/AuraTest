// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Internationalization/Text.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "GmRIS_InventoryItemDefinition.generated.h"

class UGmRIS_InventoryItemInstance;
struct FFrame;

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_InventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:

	virtual void OnInstanceCreated(UGmRIS_InventoryItemInstance* InInst) const {}
	
};

/**
 * UGmRIS_InventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_InventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	
	UGmRIS_InventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
	TArray<TObjectPtr<UGmRIS_InventoryItemFragment>> Fragments;

	const UGmRIS_InventoryItemFragment* FindFragmentByClass(TSubclassOf<UGmRIS_InventoryItemFragment> InFragmentClassRef) const;
	
};

//@TODO: Make into a subsystem instead?
UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_InventoryFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = InFragmentClassRef), Category = GmRISGlobal)
	static const UGmRIS_InventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef, TSubclassOf<UGmRIS_InventoryItemFragment> InFragmentClassRef);
	
};
