// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "GmRIS_GameplayCueManager.generated.h"

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_GameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:

	UGmRIS_GameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UGmRIS_GameplayCueManager* Get();

	//~UGameplayCueManager interface
	virtual void OnCreated() override;
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
	virtual bool ShouldSyncLoadMissingGameplayCues() const override;
	virtual bool ShouldAsyncLoadMissingGameplayCues() const override;
	//~End of UGameplayCueManager interface

	static void DumpGameplayCues(const TArray<FString>& InArgs);

	// When delay loading cues, this will load the cues that must be always loaded anyway
	void LoadAlwaysLoadedCues();

	// Updates the bundles for the singular gameplay cue primary asset
	void RefreshGameplayCuePrimaryAsset();

private:

	struct FLoadedGameplayTagToProcessData
	{
		FGameplayTag Tag;
		TWeakObjectPtr<UObject> WeakOwner;

		FLoadedGameplayTagToProcessData() {}
		FLoadedGameplayTagToProcessData(const FGameplayTag& InTag, const TWeakObjectPtr<UObject>& InWeakOwner) : Tag(InTag), WeakOwner(InWeakOwner) {}
	};

	void OnGameplayTagLoaded(const FGameplayTag& InTag);
	void HandlePostGarbageCollect();
	void ProcessLoadedTags();
	void ProcessTagToPreload(const FGameplayTag& InTag, UObject* InOwningObj);
	void OnPreloadCueComplete(FSoftObjectPath InPath, TWeakObjectPtr<UObject> InOwningObj, bool bAlwaysLoadedCue);
	void RegisterPreloadedCue(UClass* InLoadedGameplayCueClass, UObject* InOwningObj);
	void HandlePostLoadMap(UWorld* InNewWorld);
	void UpdateDelayLoadDelegateListeners();
	static bool ShouldDelayLoadGameplayCues();

	// Cues that were preloaded on the client due to being referenced by content
	UPROPERTY(Transient)
	TSet<TObjectPtr<UClass>> PreloadedCues;
	TMap<FObjectKey, TSet<FObjectKey>> PreloadedCueReferencers;

	// Cues that were preloaded on the client and will always be loaded (code referenced or explicitly always loaded)
	UPROPERTY(Transient)
	TSet<TObjectPtr<UClass>> AlwaysLoadedCues;

	TArray<FLoadedGameplayTagToProcessData> LoadedGameplayTagsToProcess;
	FCriticalSection LoadedGameplayTagsToProcessCS;
	bool bProcessLoadedTagsAfterGC = false;
	
};
