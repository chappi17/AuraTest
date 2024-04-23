// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Controller.h"
#include "GmRIS_InteractionQuery.generated.h"

USTRUCT(BlueprintType)
struct FGmRIS_InteractionQuery
{
	GENERATED_BODY()

	FGmRIS_InteractionQuery()
		:
	RequestingAvatar(nullptr),
	RequestingController(nullptr),
	OptionalObjectData(nullptr)
	{}

	FGmRIS_InteractionQuery(AActor* InActorOwner, AController* InRequestingController, const TWeakObjectPtr</*UObject*/> InOptionalObjData = nullptr)
		:
	RequestingAvatar(InActorOwner),
	RequestingController(InRequestingController),
	OptionalObjectData(InOptionalObjData)
	{}
	
	/** The requesting pawn. */
	UPROPERTY(BlueprintReadWrite, Category = InteractionQuery)
	TWeakObjectPtr<AActor> RequestingAvatar;

	/** Allow us to specify a controller - does not need to match the owner of the requesting avatar. */
	UPROPERTY(BlueprintReadWrite, Category = InteractionQuery)
	TWeakObjectPtr<AController> RequestingController;

	/** A generic UObject to shove in extra data required for the interaction */
	UPROPERTY(BlueprintReadWrite, Category = InteractionQuery)
	TWeakObjectPtr<UObject> OptionalObjectData;
};
