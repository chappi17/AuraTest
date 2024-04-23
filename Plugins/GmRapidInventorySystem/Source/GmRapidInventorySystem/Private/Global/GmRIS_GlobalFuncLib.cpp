// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "Global/GmRIS_GlobalFuncLib.h"

#include <AbilitySystemComponent.h>
#include <AbilitySystemLog.h>
#include <AudioDevice.h>
#include <Engine/GameInstance.h>
#include <Engine/Engine.h>
#include <Engine/LocalPlayer.h>
#include <GameFramework/GameStateBase.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/PlayerState.h>

#include "Equipment/GmRIS_QuickBarComponent.h"
#include "Inventory/GmRIS_StorageComponent.h"

#define LOCTEXT_NAMESPACE "GmRISGlobal"

namespace GmRISGlobal
{
	AActor* GetActorOwnerFromWorldContextObject(UObject* WorldContextObject)
	{
		if (AActor* Actor{Cast<AActor>(WorldContextObject)})
		{
			return Actor;
		}
		return WorldContextObject->GetTypedOuter<AActor>();
	}
	const AActor* GetActorOwnerFromWorldContextObject(const UObject* WorldContextObject)
	{
		if (const AActor* Actor{Cast<const AActor>(WorldContextObject)})
		{
			return Actor;
		}
		return WorldContextObject->GetTypedOuter<AActor>();
	}
}

void UGmRIS_GlobalFuncLib::GmInitAbilitySystemWithPawn(APawn* InTargetPawn, UAbilitySystemComponent* InAsc)
{
	if (!InTargetPawn || !InAsc)
	{
		UE_LOG(LogTemp, Error, L"InTargetPawn or InAsc is not valid.");
		return;
	}
	InAsc->InitAbilityActorInfo(InAsc->GetOwner(), InTargetPawn);
}

void UGmRIS_GlobalFuncLib::GmGrantedAbilities(UAbilitySystemComponent* InAsc, 
TMap<int32, TSubclassOf<UGameplayAbility>> InGameplayAbilityAndInputId, bool InActivateItImmediately)
{
	TArray<int32> Loc_Arr_Keys;
	InGameplayAbilityAndInputId.GetKeys(Loc_Arr_Keys);
	if (!InAsc)
	{
		UE_LOG(LogTemp, Error, L"Target Ability System Component is Not Valid.");
		return;
	}
	// InAsc->InitAbilityActorInfo(InAsc->GetOwner(), InPlayerPawn);

	for (int32 i{0}; i < InGameplayAbilityAndInputId.Num(); i++)
	{
		FGameplayAbilitySpec Loc_CurAbilitySpec{InAsc->BuildAbilitySpecFromClass(InGameplayAbilityAndInputId[Loc_Arr_Keys[i]], 1, Loc_Arr_Keys[i])};
		if (InActivateItImmediately)
		{
			InAsc->GiveAbilityAndActivateOnce(Loc_CurAbilitySpec);
			continue;
		}
		InAsc->GiveAbility(Loc_CurAbilitySpec);
	}
}

void UGmRIS_GlobalFuncLib::GmSimpleGrantedAbilities(UAbilitySystemComponent* InAsc,
	TArray<TSubclassOf<UGameplayAbility>> InAbilities, const bool InActivateItImmediately)
{
	if (!InAsc)
	{
		ABILITY_LOG(Error, L"Target Ability System Component is Not Valid.");
		return;
	}

	for (int32 i{0}; i < InAbilities.Num(); i++)
	{
		if (!InAbilities.IsValidIndex(i))
		{
			return;
		}
		
		FGameplayAbilitySpec Loc_NewAbilitySpec{FGameplayAbilitySpec(InAbilities[i])};
		// if (InActivateItImmediately)
		// {
			// InAsc->GiveAbilityAndActivateOnce(Loc_NewAbilitySpec);
			// continue;
		// }
		InAsc->GiveAbility(Loc_NewAbilitySpec);

		if (InActivateItImmediately)
		{
			const UGameplayAbility* const InAbilityCDO{InAbilities[i].GetDefaultObject()};
			for (const FGameplayAbilitySpec& Spec : InAsc->GetActivatableAbilities())
			{
				if (Spec.Ability == InAbilityCDO)
				{
					InAsc->TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void UGmRIS_GlobalFuncLib::GmAbilityLocalInputPressed(UAbilitySystemComponent* InAsc,
	const FGameplayTagContainer InTargetInputAbilityTag)
{
	if (!InAsc)
	{
		ABILITY_LOG(Error, L"Target Ability System Component is Not Valid.");
		return;
	}

	if (const TSharedPtr/*<FGameplayAbilityActorInfo>*/ AbilityActorInfo{InAsc->AbilityActorInfo}; !AbilityActorInfo.IsValid())
	{
		const APawn* AvatarPawn{Cast<APawn>(AbilityActorInfo->AvatarActor)};
		if (!AvatarPawn)
		{
			UE_LOG(LogTemp, Error, L"Ability Avatar actor is NULL.");
			return;
		}
		if (AbilityActorInfo->PlayerController != AvatarPawn->GetController())
		{
			UE_LOG(LogTemp, Error, L"Ability PC != Ability Avatar Pawn Controller");
			return;
		}
	}

#define ABILITYLIST_SCOPE_LOCK() FScopedAbilityListLock ActiveScopeLock(*InAsc);
	ABILITYLIST_SCOPE_LOCK();
	TArray<FGameplayAbilitySpecHandle> Loc_Arr_GASpecHandles;
	InAsc->FindAllAbilitiesWithTags(Loc_Arr_GASpecHandles, InTargetInputAbilityTag);
	
	for (const FGameplayAbilitySpecHandle Elem : Loc_Arr_GASpecHandles)
	{
		if (FGameplayAbilitySpec& Loc_Spec{*InAsc->FindAbilitySpecFromHandle(Elem)}; Loc_Spec.Ability)
		{
			Loc_Spec.InputPressed = true;
			if (Loc_Spec.IsActive())
			{
				// Server input pressed.
				if (Loc_Spec.Ability->bReplicateInputDirectly && !InAsc->IsOwnerActorAuthoritative())
				{
					InAsc->ServerSetInputPressed(Loc_Spec.Handle);
				}
				// Local input pressed.
				InAsc->AbilitySpecInputPressed(Loc_Spec);
			
				InAsc->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Elem,
					Loc_Spec.ActivationInfo.GetActivationPredictionKey());
			}
			else
			{
				InAsc->TryActivateAbility(Loc_Spec.Handle);
			}
		}
	}
}

UAudioComponent* UGmRIS_GlobalFuncLib::GmSpawnSound2D(const UObject* WorldContextObj, USoundBase* InSound,
	const float InVolumeMultiplier, const float InPitchMultiplier, const float, USoundConcurrency* InConcurrencySettings,
	const bool InPersistAcrossLevelTransition, const bool InAutoDestroy)
{
	if (!InSound || !GEngine || !GEngine->UseSound())
	{
		return nullptr;
	}

	UWorld* ThisWorld{GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull)};
	if (!ThisWorld || !ThisWorld->bAllowAudioPlayback || ThisWorld->IsNetMode(NM_DedicatedServer))
	{
		return nullptr;
	}

	// Derive an owner from the WorldContextObject
	AActor* WorldContextOwner{GmRISGlobal::GetActorOwnerFromWorldContextObject(const_cast<UObject*>(WorldContextObj))};

	FAudioDevice::FCreateComponentParams Params{InPersistAcrossLevelTransition
		? FAudioDevice::FCreateComponentParams(ThisWorld->GetAudioDeviceRaw())
		: FAudioDevice::FCreateComponentParams(ThisWorld, WorldContextOwner)};
	
	if (InConcurrencySettings)
	{
		Params.ConcurrencySet.Add(InConcurrencySettings);
	}

	UAudioComponent* AudioComponent{FAudioDevice::CreateComponent(InSound, Params)};
	if (AudioComponent)
	{
		AudioComponent->SetVolumeMultiplier(InVolumeMultiplier);
		AudioComponent->SetPitchMultiplier(InPitchMultiplier);
		AudioComponent->bAllowSpatialization = false;
		AudioComponent->bIsUISound = true;
		AudioComponent->bAutoDestroy = InAutoDestroy;
		AudioComponent->bIgnoreForFlushing = InPersistAcrossLevelTransition;
		AudioComponent->SubtitlePriority = InSound->GetSubtitlePriority();
		AudioComponent->bStopWhenOwnerDestroyed = false;
	}
	return AudioComponent;
}

AController* UGmRIS_GlobalFuncLib::GetControllerFromActorInfo(
	const FGameplayAbilityActorInfo& InCurrentActorInfo)
{
	if (const FGameplayAbilityActorInfo* Loc_CurrentActorInfo{&InCurrentActorInfo})
	{
		if (AController* PC{Loc_CurrentActorInfo->PlayerController.Get()})
		{
			return PC;
		}

		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor{Loc_CurrentActorInfo->OwnerActor.Get()};
		while (TestActor)
		{
			if (AController* C{Cast<AController>(TestActor)})
			{
				return C;
			}

			if (const APawn* Pawn{Cast<APawn>(TestActor)})
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}
	}

	return nullptr;
}

void UGmRIS_GlobalFuncLib::AddInitialInventory(AController* InTargetController,
	TArray<TSubclassOf<UGmRIS_InventoryItemDefinition>> InInitialInventoryItemDef)
{
	if (!InTargetController || InInitialInventoryItemDef.IsEmpty())
	{
		UE_LOG(LogTemp, Error, L"Add Initial Inventory : In Target Controller is Invalid or InInitialInventoryItemDef Array is Empty");
		return;
	}
	UGmRIS_StorageComponent* Loc_InventoryManagerComponent{InTargetController->FindComponentByClass<UGmRIS_StorageComponent>()};
	UGmRIS_QuickBarComponent* Loc_QuickBarComponent{InTargetController->FindComponentByClass<UGmRIS_QuickBarComponent>()};
	if (!Loc_InventoryManagerComponent || !Loc_QuickBarComponent)
	{
		UE_LOG(LogTemp, Error, L"Add Initial Inventory : Inventory Manager Component or QuickBarComponent is Invalid from Controller.");
		return;
	}
	for (int32 i{0}; i < InInitialInventoryItemDef.Num(); i++)
	{
		Loc_QuickBarComponent->AddItemToSlot(i, Loc_InventoryManagerComponent->AddItemDefinition(InInitialInventoryItemDef[i], 1, true));
	}
	Loc_QuickBarComponent->SetActiveSlotIndex(0);
}

bool UGmRIS_GlobalFuncLib::ChangeQuickBarSlot(AController* InParentController, const int32 InTargetSlotIndex,
	UAbilitySystemComponent* InTargetAbilitySystemComponent, const FGameplayTag InTargetTag)
{
	if (!InParentController || !IsValidChecked(InTargetAbilitySystemComponent))
	{
		return false;
	}
	if (const UGmRIS_QuickBarComponent* QuickBarComponent{InParentController->FindComponentByClass<UGmRIS_QuickBarComponent>()})
	{
		if (const TArray CurrentSlots{QuickBarComponent->GetSlots()}; !CurrentSlots.IsValidIndex(InTargetSlotIndex))
		{
			return false;
		}
		FGameplayEventData NewGameplayEventData;
		NewGameplayEventData.EventMagnitude = InTargetSlotIndex;
		FScopedPredictionWindow NewScopedWindow(InTargetAbilitySystemComponent, true);
		
		InTargetAbilitySystemComponent->HandleGameplayEvent(InTargetTag, &NewGameplayEventData);
		return true;
	}
	return false;
}

// void UGmRIS_GlobalFuncLib::GmStartServerTravel(const UObject* WorldContextObj, TArray<APlayerController*> InTargetPCs,
	// const TSoftObjectPtr<UWorld> InTargetLevel)
// {
// }

APlayerController* UGmRIS_GlobalFuncLib::GetPCFromSpecificIndex(const UObject* WorldContextObj, const int32 PlayerIndex)
{
	// The player controller iterator sequence varies due to map transfers and similar actions, making its index unreliable.
	// Typically, index 0 is used to refer to the primary local player controller.
	// To maintain a consistent order, especially for remote controllers, iteration should proceed with the local player controllers first,
	// followed by the GameState list for remote ones.

	const UWorld* World{GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull)};

	if (!World)
	{
		return nullptr;
	}

	// Don't use the game instance if the passed in world isn't the primary active world
	const UGameInstance* GameInstance{World->GetGameInstance()};
	const bool bUseGameInstance{GameInstance && GameInstance->GetWorld() == World};

	int32 Index{0};
	if (bUseGameInstance)
	{		
		const TArray<ULocalPlayer*>& LocalPlayers{GameInstance->GetLocalPlayers()};
		for (const ULocalPlayer* LocalPlayer : LocalPlayers)
		{
			// Only count local players with an actual PC as part of the indexing
			if (APlayerController* PC{LocalPlayer->PlayerController})
			{
				UE_LOG(LogTemp, Error, L"Index %i Player Controller is %s.", Index, *GetNameSafe(PC));
				if (Index == PlayerIndex)
				{
					return PC;
				}
				Index++;
			}
		}
	}

	// If we have a game state, use the consistent order there to pick up remote player controllers
	if (AGameStateBase* GameState{World->GetGameState()})
	{
		for (const APlayerState* PlayerState : GameState->PlayerArray)
		{
			// Ignore local player controllers we would have found in the previous pass
			if (APlayerController* PC{PlayerState ? PlayerState->GetPlayerController() : nullptr};
				PC && !(bUseGameInstance && PC->GetLocalPlayer()))
			{
				UE_LOG(LogTemp, Error, L"Index %i Player Controller is %s.", Index, *GetNameSafe(PC));
				if (Index == PlayerIndex)
				{
					return PC;
				}
				Index++;
			}
		}
	}

	// Fallback to the old behavior with a raw iterator, but only if we didn't find any potential player controllers with the other methods
	if (Index == 0)
	{
		for (FConstPlayerControllerIterator Itr{World->GetPlayerControllerIterator()}; Itr; ++Itr)
		{
			APlayerController* PlayerController{Itr->Get()};
			UE_LOG(LogTemp, Error, L"Index 0 Player Controller is %s.", *GetNameSafe(PlayerController));
			if (Index == PlayerIndex)
			{
				return PlayerController;
			}
			Index++;
		}
	}
	return nullptr;
}

void UGmRIS_GlobalFuncLib::DebugPrintControllerRoles(const AController* InTargetC, const bool InIsDebug,
	const FString InPrefixStr)
{
	if(!InTargetC)
	{
		UE_CLOG(InIsDebug, LogTemp, Error, L"%s", *FString(InPrefixStr + L" :: In Target Controller is Not valid."));
		return;
	}
	FString NetMode;
	switch (InTargetC->GetNetMode())
	{
	case NM_Client:
		NetMode = FString("NM_Client");
		break;
	case NM_Standalone:
		NetMode = FString("NM_Standalone");
		break;
	case NM_DedicatedServer:
		NetMode = FString("NM_DedicatedServer");
		break;
	case NM_ListenServer:
		NetMode = FString("NM_ListenServer");
		break;
	case NM_MAX:
		NetMode = FString("NM_MAX");
		break;
	}
	auto SimpleLambda{[&](const ENetRole InNetRole)->FString
	{
		FString RoleMode_Result;
		switch (InNetRole)
		{
		case ROLE_Authority:
			RoleMode_Result = "ROLE_Authority";
			break;
		case ROLE_None:
			RoleMode_Result = "ROLE_None";
			break;
		case ROLE_AutonomousProxy:
			RoleMode_Result = "ROLE_AutonomousProxy";
			break;
		case ROLE_SimulatedProxy:
			RoleMode_Result = "ROLE_SimulatedProxy";
			break;
		case ROLE_MAX:
			RoleMode_Result = "ROLE_MAX";
			break;
		}
		return RoleMode_Result;
	}};
	UE_CLOG(InIsDebug, LogTemp, Error, L"%s :: Target name is %s, NetMode is %s, Local Role : %s, Remote Role : %s.", *InPrefixStr,
		*GetNameSafe(InTargetC), *NetMode, *SimpleLambda(InTargetC->GetLocalRole()), *SimpleLambda(InTargetC->GetRemoteRole()));
}


#undef LOCTEXT_NAMESPACE
