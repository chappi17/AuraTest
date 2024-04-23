// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "Interaction/Tasks/GmAT_WaitForInteractableTargets.h"

#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Engine/NetSerialization.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySpec.h"
#include "SceneView.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/HUD.h"
#include "Interaction/IGmRIS_InteractableTarget.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Rotator.h"
#include "Math/UnrealMathSSE.h"
#include "Math/Vector.h"
#include "Misc/AssertionMacros.h"
#include "Physics/Experimental/PhysInterface_Chaos.h"
#include "Templates/SharedPointer.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"
#include "UObject/ScriptInterface.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

struct FGmRIS_InteractionQuery;

UGmAT_WaitForInteractableTargets::UGmAT_WaitForInteractableTargets(const FObjectInitializer& ObjectInitializer)
   :Super(ObjectInitializer)
{
}

void UGmAT_WaitForInteractableTargets::LineTrace(FHitResult& OutHitResult, const UWorld* InWorld,
   const FVector& InStart, const FVector& InEnd, const FName ProfileName, const FCollisionQueryParams& InParams)
{
   check(InWorld);

   OutHitResult = FHitResult();
   TArray<FHitResult> HitResults;
   const bool Hi{InWorld->LineTraceMultiByProfile(HitResults, InStart, InEnd, ProfileName, InParams)};

   // const FString His{Hi?"Hit":"Not Hit"};
   // UE_CLOG(Hi, LogTemp, Error, TEXT("Hit Actor is %s."), *GetNameSafe(HitResults[0].GetActor()));
   
   OutHitResult.TraceStart = InStart;
   OutHitResult.TraceEnd = InEnd;

   if (HitResults.Num() > 0)
   {
      OutHitResult = HitResults[0];
   }
}

void UGmAT_WaitForInteractableTargets::AimWithPlayerController(const AActor* InSourceActor,
   FCollisionQueryParams InParams, const FVector& InTraceStart, float InMaxRange, FVector& OutTraceEnd,
   bool) const
{
   if (!Ability) // Server and launching client only
   {
      return;
   }

   //@TODO: Bots?
   APlayerController* PC{Ability->GetCurrentActorInfo()->PlayerController.Get()};
   check(PC);

   FHitResult HitResult;
   TArray<FHitResult> HitResults;
   
   // If top down game
   if (bIsTopDown && PC->IsLocalPlayerController())
   {
      ULocalPlayer* LP{PC->GetLocalPlayer()};
      UGameViewportClient* LocalVC{LP->ViewportClient};
      bool bHit{false};
      if (LP && LocalVC)
      {
         FVector2D MousePos{FVector2D::ZeroVector};
         if (LocalVC->GetMousePosition(MousePos))
         {
            ECollisionChannel TraceChannel;
            FCollisionResponseParams ResponseParam;
            UCollisionProfile::GetChannelAndResponseParams(TraceProfile.Name, TraceChannel, ResponseParam);
            
            if (PC->GetHUD() && PC->GetHUD()->GetHitBoxAtCoordinates(MousePos, true))
            {
               bHit = false;
            }
            else if(FVector WorldOrigin, WorldDirection; UGameplayStatics::DeprojectScreenToWorld(
               PC, MousePos, WorldOrigin, WorldDirection))
            {
               bHit = FPhysicsInterface::RaycastSingle(GetWorld(), HitResult, WorldOrigin,
                  WorldOrigin + WorldDirection * PC->HitResultTraceDistance, TraceChannel,
                  FCollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), false),
                  ResponseParam);
            }
         }
      }

      if (const float InteractDist{(const float)FMath::Abs(HitResult.Location.Size() - (InTraceStart).Size())};
         bHit && InteractDist <= InteractionScanRange)
      {
         UE_CLOG(bShowDebug, LogTemp, Error, L"The current interaction distance is %f.", InteractDist);
         OutTraceEnd = HitResult.Location;
      }
      else
      {
         OutTraceEnd = InTraceStart;
      }
      
      return;
   }

   FVector ViewStart, ViewDir, ViewEnd;
   FRotator ViewRot;
   PC->GetPlayerViewPoint(ViewStart, ViewRot);

   ViewDir = ViewRot.Vector();
   ViewEnd = ViewStart + (ViewDir * InMaxRange);

   ClipCameraRayToAbilityRange(ViewStart, ViewDir, InTraceStart,
   InMaxRange, ViewEnd);
   
   // UE_LOG(LogTemp, Error, TEXT("View Start Loc X : %f, Y : %f, Z : %f."), ViewStart.X, ViewStart.Y, ViewStart.Z);
   // UE_LOG(LogTemp, Error, TEXT("View End Loc X : %f, Y : %f, Z : %f."), ViewEnd.X, ViewEnd.Y, ViewEnd.Z);
   LineTrace(HitResult, InSourceActor->GetWorld(), ViewStart, ViewEnd, TraceProfile.Name, InParams);

   const bool bUseTraceResult{HitResult.bBlockingHit && (FVector::DistSquared(InTraceStart, HitResult.Location) <=
      (powf(InMaxRange, 2)))};

   const FVector AdjustedEnd{(bUseTraceResult) ? HitResult.Location : ViewEnd};

   FVector AdjustedAimDir{(AdjustedEnd - InTraceStart).GetSafeNormal()};
   if (AdjustedAimDir.IsZero())
   {
      AdjustedAimDir = ViewDir;
   }

   if (!bTraceAffectsAimPitch && bUseTraceResult)
   {
      if (FVector OriginalAimDir{(ViewEnd - InTraceStart).GetSafeNormal()}; !OriginalAimDir.IsZero())
      {
         // Convert to angles and use original pitch
         const FRotator OriginalAimRot{OriginalAimDir.Rotation()};

         FRotator AdjustedAimRot{AdjustedAimDir.Rotation()};
         AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

         AdjustedAimDir = AdjustedAimRot.Vector();
      }
   }

   OutTraceEnd = InTraceStart + (AdjustedAimDir * InMaxRange);
}

bool UGmAT_WaitForInteractableTargets::ClipCameraRayToAbilityRange(const FVector& InCameraLocation,
   const FVector& InCameraDirection,const FVector& InAbilityCenter, const float InAbilityRange, FVector& OutClippedPosition)
{
   const FVector CameraToCenter{InAbilityCenter - InCameraLocation};
   if (const float DotToCenter{(float)FVector::DotProduct(CameraToCenter, InCameraDirection)}; DotToCenter >= 0)      //If this fails, we're pointed away from the center, but we might be inside the sphere and able to find a good exit point.
   {
      const float DistanceSquared{(float)(CameraToCenter.SizeSquared() - (powf(DotToCenter, 2)))};
      if (const float RadiusSquared{(powf(InAbilityRange, 2))}; DistanceSquared <= RadiusSquared)
      {
         const float DistanceFromCamera{FMath::Sqrt(RadiusSquared - DistanceSquared)};
         const float DistanceAlongRay{DotToCenter + DistanceFromCamera};                  //Subtracting instead of adding will get the other intersection point
         OutClippedPosition = InCameraLocation + (DistanceAlongRay * InCameraDirection);      //Cam aim point clipped to range sphere
         return true;
      }
   }
   return false;
}

void UGmAT_WaitForInteractableTargets::UpdateInteractableOptions(const FGmRIS_InteractionQuery& InInteractQuery,
   const TArray<TScriptInterface<IGmRIS_InteractableTarget>>& InInteractableTargets)
{
   TArray<FGmRIS_InteractionOpt> NewOptions;

   for (const TScriptInterface<IGmRIS_InteractableTarget>& InteractiveTarget : InInteractableTargets)
   {
      // UE_LOG(LogTemp, Error, TEXT("Interactable Target : %s."), *GetNameSafe(InInteractableTargets.GetData()->GetObject()));
      TArray<FGmRIS_InteractionOpt> TempOptions;
      FGmRIS_InteractionOptBuilder InteractionBuilder(InteractiveTarget, TempOptions);
      InteractiveTarget->GatherInteractionOptions(InInteractQuery, InteractionBuilder);

      for (FGmRIS_InteractionOpt& Option : TempOptions)
      {
         const FGameplayAbilitySpec* InteractionAbilitySpec{nullptr};

         // if there is a handle an a target ability system, we're triggering the ability on the target.
         if (Option.TargetAbilitySystem && Option.TargetInteractionAbilityHandle.IsValid())
         {
            // Find the spec
            InteractionAbilitySpec = Option.TargetAbilitySystem->FindAbilitySpecFromHandle(Option.TargetInteractionAbilityHandle);
         }
         // If there's an interaction ability then we're activating it on ourselves.
         else if (Option.InteractionAbilityToGrant)
         {
            // Find the spec
            InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Option.InteractionAbilityToGrant);

            if (InteractionAbilitySpec)
            {
               // update the option
               Option.TargetAbilitySystem = AbilitySystemComponent.Get();
               Option.TargetInteractionAbilityHandle = InteractionAbilitySpec->Handle;
            }
         }

         if (InteractionAbilitySpec)
         {
            // Filter any options that we can't activate right now for whatever reason.
            if (InteractionAbilitySpec->Ability->CanActivateAbility(InteractionAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
            {
               NewOptions.Add(Option);
            }
         }
      }
   }

   bool bOptionsChanged{false};
   if (NewOptions.Num() == CurrentOptions.Num())
   {
      NewOptions.Sort();

      for (int OptionIndex{0}; OptionIndex < NewOptions.Num(); OptionIndex++)
      {
         if (/*NewOption*/NewOptions[OptionIndex] != /*CurrentOption*/CurrentOptions[OptionIndex])
         {
            bOptionsChanged = true;
            break;
         }
      }
   }
   else
   {
      bOptionsChanged = true;
   }

   if (bOptionsChanged)
   {
      CurrentOptions = NewOptions;
      InteractableObjsChangedDelegate.Broadcast(CurrentOptions);
   }
}
