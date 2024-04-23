// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "GmRIS_SpawnedItem.generated.h"

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API AGmRIS_SpawnedItem : public AActor
{
	GENERATED_BODY()
	
public:
	
	AGmRIS_SpawnedItem(const FObjectInitializer& ObjectInitializer);

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

};
