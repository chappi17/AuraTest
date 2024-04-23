// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "GmRIS_WorldStorageActor.generated.h"

class UGmRIS_StorageManagerComponent;

UCLASS(Abstract, Blueprintable)
class GMRAPIDINVENTORYSYSTEM_API AGmRIS_WorldStorageActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	AGmRIS_WorldStorageActor(const FObjectInitializer& ObjectInitializer);

	//~ActorReplication Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of ActorReplication Interface
	
protected:

	UPROPERTY(EditAnywhere, Category = Components)
	TObjectPtr<UGmRIS_StorageManagerComponent> StorageManagerComponent;

	//~AActor Interface
	virtual void BeginPlay() override;
	//~End of AActor Interface

public:

	//~AActor Interface
	virtual void Tick(float DeltaTime) override;
	//~End of AActor Interface
	
};
