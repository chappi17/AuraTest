// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "Inventory/Item/Spawnable/GmRIS_SpawnedItem.h"

AGmRIS_SpawnedItem::AGmRIS_SpawnedItem(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGmRIS_SpawnedItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGmRIS_SpawnedItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
