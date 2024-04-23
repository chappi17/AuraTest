// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayCueNotify_Actor.h"
#include "Components/TimelineComponent.h"

#include "GmGCNA_AcquireEffect.generated.h"

class USplineComponent;
class UCurveVector;
class AActor;
class APawn;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API AGmGCNA_AcquireEffect : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:

	AGmGCNA_AcquireEffect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	//~AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~End of AActor Interface

private:

	FTimeline ItemSuckUpTransform;

	UPROPERTY()
	TObjectPtr<UCurveVector> ItemSuckUpTransformVectorCurve;

	UFUNCTION()
	void GmU_ItemSuckUpVectorUpdateFunc(FVector InTransform);

	UFUNCTION()
	void GmU_SimpleTimelineFinishedFunc();

	UPROPERTY()
	TObjectPtr<AActor> ActorToPickup{nullptr};
	UPROPERTY()
	TObjectPtr<APawn> PawnWhoPickedUp{nullptr};
	UPROPERTY()
	TObjectPtr<USplineComponent> SimpleSplineComponent{nullptr};
	
};
