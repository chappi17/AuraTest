// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GmRIS_GlobalFuncLib.generated.h"

struct FGameplayTag;
class UGmRIS_InventoryItemDefinition;
struct FGameplayAbilityActorInfo;
class AController;
class USoundConcurrency;
class USoundBase;
class UAudioComponent;
struct FGameplayTagContainer;
class UGameplayAbility;
class UAbilitySystemComponent;
class APawn;
class APlayerController;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_GlobalFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = GmRISGlobal)
	static void GmInitAbilitySystemWithPawn(APawn* InTargetPawn, UAbilitySystemComponent* InAsc);

	UFUNCTION(BlueprintCallable, Category = GmRISGlobal)
	static void GmGrantedAbilities(UAbilitySystemComponent* InAsc, TMap<int32, TSubclassOf<UGameplayAbility>> InGameplayAbilityAndInputId, bool InActivateItImmediately);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GmRISGlobal)
	static void GmSimpleGrantedAbilities(UAbilitySystemComponent* InAsc, TArray<TSubclassOf<UGameplayAbility>> InAbilities, bool InActivateItImmediately);

	UFUNCTION(BlueprintCallable, Category = GmRISGlobal)
	static void GmAbilityLocalInputPressed(UAbilitySystemComponent* InAsc, FGameplayTagContainer InTargetInputAbilityTag);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audio", meta = (WorldContext = "WorldContectObj", UnsafeDuringActorConstruction = "true", Keywords = "Sound"))
	static UAudioComponent* GmSpawnSound2D(const UObject* WorldContextObj, USoundBase* InSound, const float InVolumeMultiplier = 1.f, const float InPitchMultiplier = 1.f, const float StartTime = 0.f, USoundConcurrency* InConcurrencySettings = nullptr, const bool InPersistAcrossLevelTransition = false, const bool InAutoDestroy = true);

	UFUNCTION(BlueprintCallable, Category = GmRISGlobal)
	static AController* GetControllerFromActorInfo(const FGameplayAbilityActorInfo& InCurrentActorInfo);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GmRISGlobal, meta = (DefaultToSelf = CauserActor, HidePin = CauserActor))
	static void AddInitialInventory(AController* InTargetController, TArray<TSubclassOf<UGmRIS_InventoryItemDefinition>> InInitialInventoryItemDef);

	UFUNCTION(BlueprintCallable, Category = GmRISGlobal)
	static bool ChangeQuickBarSlot(AController* InParentController, const int32 InTargetSlotIndex, UAbilitySystemComponent* InTargetAbilitySystemComponent, FGameplayTag InTargetTag);

	// UFUNCTION(BlueprintCallable, Category = GmRISGlobal, meta = (WorldContext = "WorldContextObj", HidePin = WorldContextObj))
	// static void GmStartServerTravel(const UObject* WorldContextObj, TArray<APlayerController*> InTargetPCs, TSoftObjectPtr<UWorld> InTargetLevel);

	/** 
	 * Returns the player controller found while iterating through the local and available remote player controllers.
	 * On a network client, this will only include local players as remote player controllers are not available.
	 * The index will be consistent as long as no new players join or leave, but it will not be the same across different clients and servers.
	 *
	 * @param WorldContextObj
	 * @param PlayerIndex	Index in the player controller list, starting first with local players and then available remote ones
	 */
	UFUNCTION(BlueprintPure, Category = GmRISGlobal, meta = (WorldContext = "WorldContextObj", UnsafeDuringActorConstruction = "true"))
	static APlayerController* GetPCFromSpecificIndex(const UObject* WorldContextObj, const int32 PlayerIndex);

	UFUNCTION(BlueprintCallable, Category = GmRISGlobal)
	static void DebugPrintControllerRoles(const AController* InTargetC, const bool InIsDebug = false, const FString InPrefixStr = FString(""));

};
