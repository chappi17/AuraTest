// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include "GmRIS_EquipmentDefinition.generated.h"

class UGmRIS_EquipmentInstance;
class AActor;

USTRUCT()
struct FGmRISEquipmentActorToSpawn
{
	GENERATED_BODY()

	FGmRISEquipmentActorToSpawn()
		:
	ActorToSpawn(nullptr),
	AttachSocket(NAME_None),
	AttachTransform(FTransform())
	{}

	FGmRISEquipmentActorToSpawn(const TSubclassOf<AActor> InActorToSpawn, const FName InAttachSocket, const FTransform& InAttachTransform)
		:
	ActorToSpawn(InActorToSpawn),
	AttachSocket(InAttachSocket),
	AttachTransform(InAttachTransform)
	{}

	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttachTransform;
};

UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_EquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:

	UGmRIS_EquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category = GmRISEquipment)
	TSubclassOf<UGmRIS_EquipmentInstance> InstanceType;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category = GmRISEquipment)
	TArray<FGmRISEquipmentActorToSpawn> ActorsToSpawn;
};
