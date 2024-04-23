// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "GameplayCue/GmGCNA_AcquireEffect.h"

#include "NativeGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "Core/GmGameplayTags.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"

AGmGCNA_AcquireEffect::AGmGCNA_AcquireEffect(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinder<UCurveVector> SimpleVectorCurve;
		FConstructorStatics()
			:
		SimpleVectorCurve(L"/Script/Engine.CurveVector'/GmRapidInventorySystem/Demo/Curve/Vector/Curve_ItemSuckUpTransform.Curve_ItemSuckUpTransform'")
		{}
	};

	static FConstructorStatics GmS_Helper;
	ItemSuckUpTransformVectorCurve = GmS_Helper.SimpleVectorCurve.Object;

	GameplayCueTag = GmGameplayTags::TAG_GmRIS_GameplayCue_Interact_Pickup;
	bUniqueInstancePerInstigator = true;
}

bool AGmGCNA_AcquireEffect::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Error, L"OnExecute Gameplay Cue");
	SetActorTickEnabled(true);
	PawnWhoPickedUp = Cast<APawn>(Parameters.Instigator);
	ActorToPickup = Parameters.EffectCauser.Get();
	if (ActorToPickup)
	{
		ActorToPickup->SetActorEnableCollision(false);
		ActorToPickup->SetLifeSpan(3.0f);
		if (PawnWhoPickedUp)
		{
			SimpleSplineComponent = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
			SimpleSplineComponent->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
			FinishAddComponent(SimpleSplineComponent, false, FTransform());

			TArray<FVector> Loc_SplinePointVectors;
			Loc_SplinePointVectors.Add(ActorToPickup->GetActorLocation());
			FVector Loc_Origin, Loc_BoxExtent, Loc_ActorToPickup_Origin, Loc_ActorToPickup_BoxExtent;
			PawnWhoPickedUp->GetActorBounds(true, Loc_Origin, Loc_BoxExtent, true);
			ActorToPickup->GetActorBounds(false, Loc_ActorToPickup_Origin, Loc_ActorToPickup_BoxExtent, false);

			Loc_SplinePointVectors.Add(Loc_Origin + FVector(0, 0, Loc_BoxExtent.Z) + FVector(0, 0, Loc_ActorToPickup_BoxExtent.Z * 0.6f));
			// PawnWhoPickedUp->FindComponentByClass<USkeletalMeshComponent>()->GetSocketLocation(FName("None"))
			Loc_SplinePointVectors.Add(PawnWhoPickedUp->GetActorLocation());

			FAttachmentTransformRules Loc_AttachmentTransformRules{FAttachmentTransformRules::KeepWorldTransform};
			Loc_AttachmentTransformRules.bWeldSimulatedBodies = true;
			SimpleSplineComponent->SetSplinePoints(Loc_SplinePointVectors, ESplineCoordinateSpace::World, true);
			SimpleSplineComponent->AttachToComponent(PawnWhoPickedUp->FindComponentByClass<USkeletalMeshComponent>(), Loc_AttachmentTransformRules);

			ItemSuckUpTransform.SetPlayRate(1.0 / 0.6f);
			// ItemSuckUpTransform.SetVectorCurve(ItemSuckUpTransformVectorCurve, NAME_None);
			FOnTimelineVector OnTimelineVector;
			OnTimelineVector.BindUFunction(this, "GmU_ItemSuckUpVectorUpdateFunc");
			ItemSuckUpTransform.AddInterpVector(ItemSuckUpTransformVectorCurve, OnTimelineVector);

			FOnTimelineEvent Loc_TimelineFinishedFunc;
			Loc_TimelineFinishedFunc.BindUFunction(this, "GmU_SimpleTimelineFinishedFunc");
			ItemSuckUpTransform.SetTimelineFinishedFunc(Loc_TimelineFinishedFunc);

			UE_LOG(LogTemp, Error, L"Timeline Play From Start");
			ItemSuckUpTransform.PlayFromStart();
			
		}
		else
		{
			this->SetLifeSpan(0);
			
		}
	}
	return Super::OnExecute_Implementation(MyTarget, Parameters);
}

void AGmGCNA_AcquireEffect::Tick(const float DeltaSeconds)
{
	ItemSuckUpTransform.TickTimeline(DeltaSeconds);
	Super::Tick(DeltaSeconds);
}

void AGmGCNA_AcquireEffect::GmU_ItemSuckUpVectorUpdateFunc(FVector InTransform)
{
	UE_LOG(LogTemp, Error, L"Suck Up");
	USceneComponent* TargetRootComponent{ActorToPickup->GetRootComponent()};
	if (!ActorToPickup || !PawnWhoPickedUp || !SimpleSplineComponent)
	{
		return;
	}
	TargetRootComponent->SetWorldLocation(
		FVector(SimpleSplineComponent->GetLocationAtDistanceAlongSpline(
			InTransform.X*(SimpleSplineComponent->GetSplineLength())
			/*FMath::Lerp(0.0f, SimpleSplineComponent->GetSplineLength(), InTransform.X)*/,
			ESplineCoordinateSpace::World)
			),
			false);
	TargetRootComponent->SetWorldScale3D(FVector(InTransform.Y) * TargetRootComponent->GetComponentScale());
}

void AGmGCNA_AcquireEffect::GmU_SimpleTimelineFinishedFunc()
{
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SetLifeSpan(3.0f);
}
