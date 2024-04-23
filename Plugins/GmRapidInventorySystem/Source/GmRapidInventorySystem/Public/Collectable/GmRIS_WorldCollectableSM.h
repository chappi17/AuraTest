// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GmRIS_WorldCollectableObject.h"

#include "GmRIS_WorldCollectableSM.generated.h"

UCLASS(Blueprintable)
class GMRAPIDINVENTORYSYSTEM_API AGmRIS_WorldCollectableSM : public AGmRIS_WorldCollectableObject
{
	GENERATED_BODY()
	
public:
	
	AGmRIS_WorldCollectableSM(const FObjectInitializer& ObjectInitializer);

	virtual void SetMesh(UStreamableRenderAsset* InNewMesh) override;
	
	virtual TObjectPtr<UMeshComponent>& GetMeshComp() override
	{
		return Super::GetMeshComp();
	}
	
	//~AActor Interface
	// virtual void OnConstruction(const FTransform& Transform) override;
	//~End of AActor Interface
	
};
