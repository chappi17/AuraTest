// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "AbilitySystem/GmRIS_GameplayCueManager.h"

#include "GameplayCueSet.h"
#include "AbilitySystemGlobals.h"
#include "HAL/IConsoleManager.h"
#include "GameplayTagsManager.h"
#include "UObject/UObjectThreadContext.h"
#include "Engine/AssetManager.h"
#include "Async/Async.h"
#include "Algo/Transform.h"

enum class EGmRISEditorLoadMode
{
	// Loads all cues upfront; longer loading speed in the editor but short PIE times and effects never fail to play
	LoadUpfront,

	// Outside of editor: Async loads as cue tag are registered
	// In editor: Async loads when cues are invoked
	//   Note: This can cause some 'why didn't I see the effect for X' issues in PIE and is good for iteration speed but otherwise bad for designers
	PreloadAsCuesAreReferenced_GameOnly,

	// Async loads as cue tag are registered
	PreloadAsCuesAreReferenced
};

namespace GmRISGameplayCueManagerCvars
{
	static FAutoConsoleCommand CVarDumpGameplayCues(
		L"GmRIS.DumpGameplayCues",
		L"Shows all assets that were loaded via GmRISGameplayCueManager and are currently in memory.",
		FConsoleCommandWithArgsDelegate::CreateStatic(UGmRIS_GameplayCueManager::DumpGameplayCues));

	static EGmRISEditorLoadMode LoadMode = EGmRISEditorLoadMode::LoadUpfront;
}

struct FGameplayCueTagThreadSynchronizeGraphTask : public FAsyncGraphTaskBase
{
	TFunction<void()> TheTask;
	FGameplayCueTagThreadSynchronizeGraphTask(TFunction<void()>&& Task) : TheTask(MoveTemp(Task)) { }
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) { TheTask(); }
	ENamedThreads::Type GetDesiredThread() { return ENamedThreads::GameThread; }
};

UGmRIS_GameplayCueManager::UGmRIS_GameplayCueManager(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UGmRIS_GameplayCueManager* UGmRIS_GameplayCueManager::Get()
{
	return Cast<UGmRIS_GameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

void UGmRIS_GameplayCueManager::OnCreated()
{
	Super::OnCreated();
}

bool UGmRIS_GameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
	return Super::ShouldAsyncLoadRuntimeObjectLibraries();
}

bool UGmRIS_GameplayCueManager::ShouldSyncLoadMissingGameplayCues() const
{
	return Super::ShouldSyncLoadMissingGameplayCues();
}

bool UGmRIS_GameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const
{
	return Super::ShouldAsyncLoadMissingGameplayCues();
}

void UGmRIS_GameplayCueManager::DumpGameplayCues(const TArray<FString>& InArgs)
{
	UGmRIS_GameplayCueManager* GCM{Cast<UGmRIS_GameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager())};
	if (!GCM)
	{
		UE_LOG(LogTemp, Error, L"DumpGameplayCues failed. No UGmRIS_GameplayCueManager found.");
		return;
	}

	const bool bIncludeRefs{InArgs.Contains(L"Refs")};

	UE_LOG(LogTemp, Log, L"=========== Dumping Always Loaded Gameplay Cue Notifies ===========");
	for (const UClass* CueClass : GCM->AlwaysLoadedCues)
	{
		UE_LOG(LogTemp, Log, L"  %s", *GetPathNameSafe(CueClass));
	}

	UE_LOG(LogTemp, Log, L"=========== Dumping Preloaded Gameplay Cue Notifies ===========");
	for (UClass* CueClass : GCM->PreloadedCues)
	{
		TSet<FObjectKey>* ReferencerSet{GCM->PreloadedCueReferencers.Find(CueClass)};
		int32 NumRefs{ReferencerSet ? ReferencerSet->Num() : 0};
		UE_LOG(LogTemp, Log, L"  %s (%d refs)", *GetPathNameSafe(CueClass), NumRefs);
		if (bIncludeRefs && ReferencerSet)
		{
			for (const FObjectKey& Ref : *ReferencerSet)
			{
				const UObject* RefObject{Ref.ResolveObjectPtr()};
				UE_LOG(LogTemp, Log, L"    ^- %s", *GetPathNameSafe(RefObject));
			}
		}
	}

	UE_LOG(LogTemp, Log, L"=========== Dumping Gameplay Cue Notifies loaded on demand ===========");
	int32 NumMissingCuesLoaded{0};
	if (GCM->RuntimeGameplayCueObjectLibrary.CueSet)
	{
		for (const FGameplayCueNotifyData& CueData : GCM->RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData)
		{
			if (CueData.LoadedGameplayCueClass && !GCM->AlwaysLoadedCues.Contains(CueData.LoadedGameplayCueClass) && !GCM->PreloadedCues.Contains(CueData.LoadedGameplayCueClass))
			{
				NumMissingCuesLoaded++;
				UE_LOG(LogTemp, Log, L"  %s", *CueData.LoadedGameplayCueClass->GetPathName());
			}
		}
	}

	UE_LOG(LogTemp, Log, L"=========== Gameplay Cue Notify summary ===========");
	UE_LOG(LogTemp, Log, L"  ... %d cues in always loaded list", GCM->AlwaysLoadedCues.Num());
	UE_LOG(LogTemp, Log, L"  ... %d cues in preloaded list", GCM->PreloadedCues.Num());
	UE_LOG(LogTemp, Log, L"  ... %d cues loaded on demand", NumMissingCuesLoaded);
	UE_LOG(LogTemp, Log, L"  ... %d cues in total", GCM->AlwaysLoadedCues.Num() + GCM->PreloadedCues.Num() + NumMissingCuesLoaded);
}

void UGmRIS_GameplayCueManager::LoadAlwaysLoadedCues()
{
	if (ShouldDelayLoadGameplayCues())
	{
		const UGameplayTagsManager& TagManager{UGameplayTagsManager::Get()};
	
		//@TODO: Try to collect these by filtering GameplayCue. tags out of native gameplay tags?
		TArray<FName> AdditionalAlwaysLoadedCueTags;

		for (const FName& CueTagName : AdditionalAlwaysLoadedCueTags)
		{
			if (FGameplayTag CueTag{TagManager.RequestGameplayTag(CueTagName, /*ErrorIfNotFound=*/ false)}; CueTag.IsValid())
			{
				ProcessTagToPreload(CueTag, nullptr);
			}
			else
			{
				UE_LOG(LogTemp, Warning, L"UGmRIS_GameplayCueManager::AdditionalAlwaysLoadedCueTags contains invalid tag %s", *CueTagName.ToString());
			}
		}
	}
}

void UGmRIS_GameplayCueManager::OnGameplayTagLoaded(const FGameplayTag& InTag)
{
	FScopeLock ScopeLock(&LoadedGameplayTagsToProcessCS);
	const bool bStartTask{LoadedGameplayTagsToProcess.Num() == 0};
	const FUObjectSerializeContext* LoadContext{FUObjectThreadContext::Get().GetSerializeContext()};
	UObject* OwningObject{LoadContext ? LoadContext->SerializedObject : nullptr};
	LoadedGameplayTagsToProcess.Emplace(InTag, OwningObject);
	if (bStartTask)
	{
		TGraphTask<FGameplayCueTagThreadSynchronizeGraphTask>::CreateTask().ConstructAndDispatchWhenReady([]()->void
			{
				if (GIsRunning)
				{
					if (UGmRIS_GameplayCueManager* StrongThis{Get()})
					{
						// If we are garbage collecting we cannot call StaticFindObject (or a few other static uobject functions), so we'll just wait until the GC is over and process the tags then
						if (IsGarbageCollecting())
						{
							StrongThis->bProcessLoadedTagsAfterGC = true;
						}
						else
						{
							StrongThis->ProcessLoadedTags();
						}
					}
				}
			});
	}
}

void UGmRIS_GameplayCueManager::ProcessLoadedTags()
{
	TArray<FLoadedGameplayTagToProcessData> TaskLoadedGameplayTagsToProcess;
	{
		// Lock LoadedGameplayTagsToProcess just long enough to make a copy and clear
		FScopeLock TaskScopeLock(&LoadedGameplayTagsToProcessCS);
		TaskLoadedGameplayTagsToProcess = LoadedGameplayTagsToProcess;
		LoadedGameplayTagsToProcess.Empty();
	}

	// This might return during shutdown, and we don't want to proceed if that is the case
	if (GIsRunning)
	{
		if (RuntimeGameplayCueObjectLibrary.CueSet)
		{
			for (const FLoadedGameplayTagToProcessData& LoadedTagData : TaskLoadedGameplayTagsToProcess)
			{
				if (RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Contains(LoadedTagData.Tag))
				{
					if (!LoadedTagData.WeakOwner.IsStale())
					{
						ProcessTagToPreload(LoadedTagData.Tag, LoadedTagData.WeakOwner.Get());
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, L"UGmRIS_GameplayCueManager::OnGameplayTagLoaded processed loaded tag(s) but RuntimeGameplayCueObjectLibrary.CueSet was null. Skipping processing.");
		}
	}
}

void UGmRIS_GameplayCueManager::ProcessTagToPreload(const FGameplayTag& InTag, UObject* InOwningObj)
{
	switch (GmRISGameplayCueManagerCvars::LoadMode)
	{
	case EGmRISEditorLoadMode::LoadUpfront:
		return;
	case EGmRISEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		if (GIsEditor)
		{
			return;
		}
#endif
		break;
	case EGmRISEditorLoadMode::PreloadAsCuesAreReferenced:
		break;
	}

	check(RuntimeGameplayCueObjectLibrary.CueSet);

	if (const int32* DataIdx{RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Find(InTag)}; DataIdx && RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData.IsValidIndex(*DataIdx))
	{
		const FGameplayCueNotifyData& CueData{RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData[*DataIdx]};

		if (UClass* LoadedGameplayCueClass{FindObject<UClass>(nullptr, *CueData.GameplayCueNotifyObj.ToString())})
		{
			RegisterPreloadedCue(LoadedGameplayCueClass, InOwningObj);
		}
		else
		{
			// bool bAlwaysLoadedCue = ;
			const TWeakObjectPtr/*<UObject>*/ WeakOwner{InOwningObj};
			StreamableManager.RequestAsyncLoad(CueData.GameplayCueNotifyObj, FStreamableDelegate::CreateUObject(this, &ThisClass::OnPreloadCueComplete, CueData.GameplayCueNotifyObj, WeakOwner, /*bAlwaysLoadedCue*/!InOwningObj), FStreamableManager::DefaultAsyncLoadPriority, false, false, L"GameplayCueManager");
		}
	}
}

void UGmRIS_GameplayCueManager::OnPreloadCueComplete(FSoftObjectPath InPath, TWeakObjectPtr<UObject> InOwningObj,
	bool bAlwaysLoadedCue)
{
	if (bAlwaysLoadedCue || InOwningObj.IsValid())
	{
		if (UClass* LoadedGameplayCueClass{Cast<UClass>(InPath.ResolveObject())})
		{
			RegisterPreloadedCue(LoadedGameplayCueClass, InOwningObj.Get());
		}
	}
}

void UGmRIS_GameplayCueManager::RegisterPreloadedCue(UClass* InLoadedGameplayCueClass, UObject* InOwningObj)
{
	check(InLoadedGameplayCueClass);

	if (/*AlwaysLoadedCue*/InOwningObj == nullptr)
	{
		AlwaysLoadedCues.Add(InLoadedGameplayCueClass);
		PreloadedCues.Remove(InLoadedGameplayCueClass);
		PreloadedCueReferencers.Remove(InLoadedGameplayCueClass);
	}
	else if ((InOwningObj != InLoadedGameplayCueClass) && (InOwningObj != InLoadedGameplayCueClass->GetDefaultObject()) && !AlwaysLoadedCues.Contains(InLoadedGameplayCueClass))
	{
		PreloadedCues.Add(InLoadedGameplayCueClass);
		TSet<FObjectKey>& ReferencerSet{PreloadedCueReferencers.FindOrAdd(InLoadedGameplayCueClass)};
		ReferencerSet.Add(InOwningObj);
	}
}

void UGmRIS_GameplayCueManager::HandlePostGarbageCollect()
{
	if (bProcessLoadedTagsAfterGC)
	{
		ProcessLoadedTags();
	}
	bProcessLoadedTagsAfterGC = false;
}

void UGmRIS_GameplayCueManager::HandlePostLoadMap(UWorld*)
{
	if (RuntimeGameplayCueObjectLibrary.CueSet)
	{
		for (UClass* CueClass : AlwaysLoadedCues)
		{
			RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
		}

		for (UClass* CueClass : PreloadedCues)
		{
			RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
		}
	}

	for (auto CueIt{PreloadedCues.CreateIterator()}; CueIt; ++CueIt)
	{
		TSet<FObjectKey>& ReferencerSet{PreloadedCueReferencers.FindChecked(*CueIt)};
		for (auto RefIt{ReferencerSet.CreateIterator()}; RefIt; ++RefIt)
		{
			if (!RefIt->ResolveObjectPtr())
			{
				RefIt.RemoveCurrent();
			}
		}
		if (ReferencerSet.Num() == 0)
		{
			PreloadedCueReferencers.Remove(*CueIt);
			CueIt.RemoveCurrent();
		}
	}
}

void UGmRIS_GameplayCueManager::UpdateDelayLoadDelegateListeners()
{
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.RemoveAll(this);
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	switch (GmRISGameplayCueManagerCvars::LoadMode)
	{
	case EGmRISEditorLoadMode::LoadUpfront:
		return;
	case EGmRISEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		if (GIsEditor)
		{
			return;
		}
#endif
		break;
	case EGmRISEditorLoadMode::PreloadAsCuesAreReferenced:
		break;
	}

	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.AddUObject(this, &ThisClass::OnGameplayTagLoaded);
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &ThisClass::HandlePostGarbageCollect);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
}

bool UGmRIS_GameplayCueManager::ShouldDelayLoadGameplayCues()
{
	return !IsRunningDedicatedServer()/* && ClientDelayLoadGameplayCues*/;
}

const FPrimaryAssetType UFortAssetManager_GameplayCueRefsType{L"GameplayCueRefs"};
const FName UFortAssetManager_GameplayCueRefsName{L"GameplayCueReferences"};
const FName UFortAssetManager_LoadStateClient{FName(L"Client")};

void UGmRIS_GameplayCueManager::RefreshGameplayCuePrimaryAsset()
{
	TArray<FSoftObjectPath> CuePaths;
	if (const UGameplayCueSet* RuntimeGameplayCueSet{GetRuntimeCueSet()})
	{
		RuntimeGameplayCueSet->GetSoftObjectPaths(CuePaths);
	}

	FAssetBundleData BundleData;
	BundleData.AddBundleAssetsTruncated(UFortAssetManager_LoadStateClient, CuePaths);

	UAssetManager::Get().AddDynamicAsset(/*PrimaryAssetId*/FPrimaryAssetId(UFortAssetManager_GameplayCueRefsType, UFortAssetManager_GameplayCueRefsName), FSoftObjectPath(), BundleData);
}