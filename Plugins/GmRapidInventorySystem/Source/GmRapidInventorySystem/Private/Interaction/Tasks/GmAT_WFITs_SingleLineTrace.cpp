// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "Interaction/Tasks/GmAT_WFITs_SingleLineTrace.h"

#include "GameFramework/Actor.h"
#include "Interaction/IGmRIS_InteractableTarget.h"
#include "Interaction/GmRIS_InteractionStatics.h"
#include "Interaction/GmRIS_InteractionQuery.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"

UGmAT_WFITs_SingleLineTrace::UGmAT_WFITs_SingleLineTrace(const FObjectInitializer& ObjectInitializer)
   :Super(ObjectInitializer)
{}

void UGmAT_WFITs_SingleLineTrace::Activate()
{
   SetWaitingOnAvatar();

   GetWorld()->GetTimerManager().SetTimer(GmActiveTH, this, &ThisClass::ActivateTrace, InteractionScanRate, true);
}

UGmAT_WFITs_SingleLineTrace* UGmAT_WFITs_SingleLineTrace::WFITs_SingleLineTrace(UGameplayAbility* OwningAbility,
   const FGmRIS_InteractionQuery InInteractionQuery, const FCollisionProfileName TraceProfile,
   const FGameplayAbilityTargetingLocationInfo StartLocation, const float InteractionScanRange,
   const float InteractionScanRate,const bool InShowDebug, const bool InIsTopDown)
{
   UGmAT_WFITs_SingleLineTrace* Loc_MyObj{NewAbilityTask<UGmAT_WFITs_SingleLineTrace>(OwningAbility)};
   Loc_MyObj->InteractionScanRange = InteractionScanRange;
   Loc_MyObj->InteractionScanRate = InteractionScanRate;
   Loc_MyObj->StartLocation = StartLocation;
   Loc_MyObj->InteractionQuery = InInteractionQuery;
   Loc_MyObj->TraceProfile = TraceProfile;
   Loc_MyObj->bShowDebug = InShowDebug;
   Loc_MyObj->bIsTopDown = InIsTopDown;

   return Loc_MyObj;
}

void UGmAT_WFITs_SingleLineTrace::OnDestroy(const bool bInOwnerFinished)
{
   if (const UWorld* Loc_CurWorld{GetWorld()})
   {
      Loc_CurWorld->GetTimerManager().ClearTimer(GmActiveTH);
   }

   Super::OnDestroy(bInOwnerFinished);
}

void UGmAT_WFITs_SingleLineTrace::ActivateTrace()
{
   AActor* AvatarActor{Ability->GetCurrentActorInfo()->AvatarActor.Get()};
   
   if (!AvatarActor)
   {
      return;
   }

   UWorld* World{GetWorld()};

   TArray<AActor*> ActorsToIgnore;
   ActorsToIgnore.Add(AvatarActor);

   constexpr bool bTraceComplex{false};
   FCollisionQueryParams Params(SCENE_QUERY_STAT(UGmAT_WFITs_SingleLineTrace), bTraceComplex);
   Params.AddIgnoredActors(ActorsToIgnore);

   FVector TraceStart{StartLocation.GetTargetingTransform().GetLocation()}, TraceEnd{FVector::ZeroVector};
   
   AimWithPlayerController(AvatarActor, Params, TraceStart, InteractionScanRange, OUT TraceEnd);
   
   // UE_LOG(LogTemp, Error, TEXT("Trace Start Location :: X:%f, Y:%f, Z:%f."), TraceStart.X, TraceStart.Y, TraceStart.Z);
   // UE_LOG(LogTemp, Error, TEXT("Trace End Location :: X:%f, Y:%f, Z:%f."), TraceEnd.X, TraceEnd.Y, TraceEnd.Z);
   FHitResult OutHitResult;
   LineTrace(OutHitResult, World, TraceStart, TraceEnd, TraceProfile.Name, Params);
   
   TArray<TScriptInterface<IGmRIS_InteractableTarget>> InteractableTargets;
   UGmRIS_InteractionStatics::AppendInteractableTargetsFromHitResult(OutHitResult, InteractableTargets);

   UpdateInteractableOptions(InteractionQuery, InteractableTargets);

#if ENABLE_DRAW_DEBUG
   if (bShowDebug)
   {
      FColor DebugColor{OutHitResult.bBlockingHit ? FColor::Red : FColor::Green};
      if (OutHitResult.bBlockingHit)
      {
         DrawDebugLine(World, TraceStart, OutHitResult.Location, DebugColor, false, InteractionScanRate);
         DrawDebugSphere(World, OutHitResult.Location, 5, 16, DebugColor, false, InteractionScanRate);
      }
      else
      {
         DrawDebugLine(World, TraceStart, TraceEnd, DebugColor, false, InteractionScanRate);
      }
   }
#endif // ENABLE_DRAW_DEBUG
}