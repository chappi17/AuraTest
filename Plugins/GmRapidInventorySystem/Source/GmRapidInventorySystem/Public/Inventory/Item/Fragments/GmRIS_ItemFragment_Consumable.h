// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"

#include "GmRIS_ItemFragment_Consumable.generated.h"

class UStreamableRenderAsset;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_ItemFragment_Consumable : public UGmRIS_InventoryItemFragment
{
	GENERATED_BODY()

public:

	// Enable consumable?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Consume)
	bool bIsConsumable{true};

	// Enable drop?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drop)
	bool bCanDrop{true};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drop, meta = (DisplayThumbnail = true, DisplayName = "Mesh to be Dropped", AllowedClasses = "/Script/Engine.StaticMesh,/Script/Engine.SkeletalMesh"))
	TObjectPtr<UStreamableRenderAsset> MeshToBeDropped;
	
};
