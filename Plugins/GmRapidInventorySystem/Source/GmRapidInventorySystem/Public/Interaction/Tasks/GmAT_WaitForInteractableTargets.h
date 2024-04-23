// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "Containers/Array.h"
#include "Delegates/Delegate.h"
#include "Engine/CollisionProfile.h"
#include "Interaction/GmRIS_InteractionOpt.h"
#include "Math/MathFwd.h"
#include "UObject/NameTypes.h"

#include "GmAT_WaitForInteractableTargets.generated.h"

struct FGmRIS_InteractionQuery;
class AActor;
class IGmRIS_InteractableTarget;
class UObject;
class UWorld;
struct FCollisionQueryParams;
struct FHitResult;
struct FInteractionQuery;
template <typename InterfaceType> class TScriptInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGmRIS_InteractableObjsChangedEvent, const TArray<FGmRIS_InteractionOpt>&, InteractableOpts);

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmAT_WaitForInteractableTargets : public UAbilityTask
{
   GENERATED_BODY()

public:

   UGmAT_WaitForInteractableTargets(const FObjectInitializer& ObjectInitializer);

   UPROPERTY(BlueprintAssignable)
   FGmRIS_InteractableObjsChangedEvent InteractableObjsChangedDelegate;

protected:

   static void LineTrace(FHitResult& OutHitResult, const UWorld* InWorld, const FVector& InStart, const FVector& InEnd, FName ProfileName, const FCollisionQueryParams& InParams);

   void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams InParams, const FVector& InTraceStart, float InMaxRange, FVector& OutTraceEnd, bool InIgnorePitch = false) const;

   static bool ClipCameraRayToAbilityRange(const FVector& InCameraLocation, const FVector& InCameraDirection, const FVector& InAbilityCenter, float InAbilityRange, FVector& OutClippedPosition);

   void UpdateInteractableOptions(const FGmRIS_InteractionQuery& InInteractQuery, const TArray<TScriptInterface<IGmRIS_InteractableTarget>>& InInteractableTargets);

   FCollisionProfileName TraceProfile;

   // Does the trace affect the aiming pitch
   bool bTraceAffectsAimPitch{true};
   bool bIsTopDown{false};
   bool bShowDebug{false};

   TArray<FGmRIS_InteractionOpt> CurrentOptions;

   float InteractionScanRange{100};
   
};
