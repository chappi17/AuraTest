// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Collectable/GmRIS_WorldStorageActor.h"

#include "Inventory/GmRIS_StorageManagerComponent.h"

AGmRIS_WorldStorageActor::AGmRIS_WorldStorageActor(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	bAlwaysRelevant = true;
	ThisClass::SetReplicateMovement(true);

	StorageManagerComponent = CreateDefaultSubobject<UGmRIS_StorageManagerComponent>(L"StorageManagerComponent");
}

void AGmRIS_WorldStorageActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool AGmRIS_WorldStorageActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}

void AGmRIS_WorldStorageActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGmRIS_WorldStorageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
