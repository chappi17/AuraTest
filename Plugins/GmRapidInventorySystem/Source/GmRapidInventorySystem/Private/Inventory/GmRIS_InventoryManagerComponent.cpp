// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Inventory/GmRIS_InventoryManagerComponent.h"

#include <Components/AudioComponent.h>
#include <Engine/ActorChannel.h>
#include <GameFramework/Actor.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectPtr.h>
#include <UObject/ConstructorHelpers.h>
#include <Misc/AssertionMacros.h>

#include "Collectable/GmRIS_WorldCollectableSM.h"
#include "Core/GmGameplayTags.h"
#include "GameFramework/GameplayMsgSubsystem.h"
#include "Global/GmRIS_GlobalFuncLib.h"
#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "Inventory/GmRIS_InventoryItemInstance.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_Consumable.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_InventoryTile.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_SetStats.h"
#include "UI/Container/GmRISW_InventoryContainer.h"
#include "Equipment/GmRIS_QuickBarComponent.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_NotificationInfo.h"
#include "UI/GmRISW_InventoryPanel.h"
#include "UI/Container/GmRISW_MainLayout.h"

///////////////////////////////////////////////////////////////
/// UGmRIS_InventoryManagerComponent
/// 
UGmRIS_InventoryManagerComponent::UGmRIS_InventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	if (!MainLayoutWidgetClassRef)
	{
		struct FConstructorStatics
		{
			ConstructorHelpers::FClassFinder<UGmRISW_MainLayout> DefaultLayoutClassPath;
			FConstructorStatics()
				:
			DefaultLayoutClassPath(L"/Script/UMGEditor.WidgetBlueprint'/GmRapidInventorySystem/Demo/UI/Hud/W_MainHud'")
			{}
		};

		static FConstructorStatics GmS_Helper;

		if (GmS_Helper.DefaultLayoutClassPath.Succeeded())
		{
			MainLayoutWidgetClassRef = GmS_Helper.DefaultLayoutClassPath.Class;
		}
	}
}

void UGmRIS_InventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* CurPC{Cast<APlayerController>(GetOuter())};
		CurPC && CurPC->IsLocalPlayerController())
	{
		UGameplayMsgSubsystem::Get(this).RegisterListener(
			GmGameplayTags::TAG_GmRIS_Inventory_Item_Using, this, &ThisClass::ItemHasBeenUsed);

		if (MainLayoutWidgetClassRef)
		{
			GmMainLayoutWidgetCache = CreateWidget<UGmRISW_MainLayout>(CurPC, MainLayoutWidgetClassRef);
			GmMainLayoutWidgetCache->AddToPlayerScreen(0);
		}
	}
}

void UGmRIS_InventoryManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GmMainLayoutWidgetCache)
	{
		GmMainLayoutWidgetCache->RemoveFromParent();
	}
	
	Super::EndPlay(EndPlayReason);
}

bool UGmRIS_InventoryManagerComponent::ConsumeItemsByDefinition(const TSubclassOf<UGmRIS_InventoryItemDefinition> InItemDef,
	const int32 InNumToConsume)
{
	if (const AActor* OwningActor{GetOwner()};
		!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed{0};
	
	while (TotalConsumed < InNumToConsume)
	{
		if (UGmRIS_InventoryItemInstance* Instance{/*UGmRIS_InventoryManagerComponent::*/FindFirstItemStackByDefinition(InItemDef)})
		{
			Storage.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == InNumToConsume;
}

void UGmRIS_InventoryManagerComponent::GmSetInventoryMainWidget(UGmRISW_InventoryContainer* InNewInventoryWidgetRef)
{
	APlayerController* CurPc{Cast<APlayerController>(GetOuter())};
	
	if (!CurPc)
	{
		UE_CLOG(bIsDebug, LogTemp, Error, L"Current owner is not APlayerController.");
		return;
	}

	if (!CurPc->IsLocalPlayerController()) return;

	if (InNewInventoryWidgetRef)
	{
		if(!InNewInventoryWidgetRef->GetOwningPlayer())
		{
			// UE_LOG(LogTemp, Error, L"GmSetInventoryMainWidget() : SetOwningPlayer()");
			InNewInventoryWidgetRef->SetOwningPlayer(CurPc);
		}
		
		FInputModeGameAndUI Loc_InputMode;
		Loc_InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Loc_InputMode.SetWidgetToFocus(InNewInventoryWidgetRef->TakeWidget());

		Loc_InputMode.SetHideCursorDuringCapture(false);
		CurPc->SetInputMode(Loc_InputMode);
		CurPc->SetShowMouseCursor(true);
		
		InNewInventoryWidgetRef->GmSimpleInventoryGrid->ReGenerateItemTileView(GetAllItems());
		InNewInventoryWidgetRef->AddToPlayerScreen(99);
	}
	else if(GmMainInventoryWidgetCache)
	{
		CurPc->SetInputMode(FInputModeGameOnly());
		CurPc->SetShowMouseCursor(false);
		
		GmMainInventoryWidgetCache->RemoveFromParent();
	}
	
	GmMainInventoryWidgetCache = InNewInventoryWidgetRef;
	
	UE_CLOG(bIsDebug, LogTemp, Error, L"Current owner APlayerController is %s.", *GetNameSafe(CurPc));
}

void UGmRIS_InventoryManagerComponent::Server_ItemHasBeenUsed_Implementation(
	const FGmSimpleItemInstance& InUsedItemInst)
{
	Internal_ItemHasBeenUsed(InUsedItemInst);
}

void UGmRIS_InventoryManagerComponent::ItemHasBeenUsed_Implementation(FGameplayTag,
	const FGmSimpleItemInstance& InUsedItemInst)
{
	UGmRIS_GlobalFuncLib::DebugPrintControllerRoles(
		Cast<AController>(GetOwner()), true, FString("Client_ItemHasBeenUsed()"));
	
	if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		Server_ItemHasBeenUsed(InUsedItemInst);
	}
	else if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		Server_ItemHasBeenUsed(InUsedItemInst);
		Internal_ItemHasBeenUsed(InUsedItemInst);
	}
}

void UGmRIS_InventoryManagerComponent::Internal_ItemHasBeenUsed(const FGmSimpleItemInstance& InUsedItemInst)
{
	if (!InUsedItemInst.ItemInst)
	{
		UE_CLOG(bIsDebug, LogTemp, Error, L"The requested item does not exist.");
		return;
	}
	
	// Consumable fragment
	if (const UGmRIS_ItemFragment_Consumable* ConsumableInfo{
		InUsedItemInst.ItemInst->FindFragmentByClass<UGmRIS_ItemFragment_Consumable>()})
	{
		const UGmRIS_ItemFragment_InventoryTile* Loc_ItemInventoryTileInfo{
			InUsedItemInst.ItemInst->FindFragmentByClass<UGmRIS_ItemFragment_InventoryTile>()};
		
		if (ConsumableInfo->bIsConsumable && !InUsedItemInst.bIsDrop)
		{
			if (Loc_ItemInventoryTileInfo)
			{
				// Play using sound.
				if (UAudioComponent* Loc_UsingAudioComponent{UGmRIS_GlobalFuncLib::GmSpawnSound2D(
					GetOwner(), Loc_ItemInventoryTileInfo->ItemSoundOptions.SoundWhenUsingAnItem,
					Loc_ItemInventoryTileInfo->ItemSoundOptions.UsingItemSoundVolumeAndPitch.X,
					Loc_ItemInventoryTileInfo->ItemSoundOptions.UsingItemSoundVolumeAndPitch.Y)})
				{
					Loc_UsingAudioComponent->Play(0.f);
				}
			}

			// Checks whether the current quantity of an item is 0.
			if (InUsedItemInst.ItemInst->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity) < 1)
			{
				UE_CLOG(bIsDebug, LogTemp, Error, L"Item will be remove");
				
				// Internally, completely remove item instances from the inventory manager component.
				RemoveItemInstance(InUsedItemInst.ItemInst);
			}
			else if (const UGmRIS_ItemFragment_SetStats* Loc_ItemStatInfo{
				InUsedItemInst.ItemInst->FindFragmentByClass<UGmRIS_ItemFragment_SetStats>()})
			{
				InUsedItemInst.ItemInst->RemoveStatTagStack(
					GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity, InUsedItemInst.bIsAllQuantities ?
					InUsedItemInst.ItemInst->GetStatTagStackCount(
						GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity) : Loc_ItemStatInfo->GetNumberConsumedPerTime());
			}
		}
		else if(ConsumableInfo->bCanDrop && InUsedItemInst.bIsDrop)
		{
			if (Loc_ItemInventoryTileInfo)
			{
				// Remove stat tag stack.
				//@TODO Is right?
				// InUsedItemInst.ItemInst->RemoveStatTagStack(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity, InUsedItemInst.ItemInst->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));

				// Play using sound.
				if (UAudioComponent* Loc_DropAudioComponent{UGmRIS_GlobalFuncLib::GmSpawnSound2D(
					GetWorld(), Loc_ItemInventoryTileInfo->ItemSoundOptions.SoundWhenDroppingAnItem,
					Loc_ItemInventoryTileInfo->ItemSoundOptions.DroppingItemSoundVolumeAndPitch.X,
					Loc_ItemInventoryTileInfo->ItemSoundOptions.DroppingItemSoundVolumeAndPitch.Y)})
				{
					Loc_DropAudioComponent->Play(0.f);
				}
			}

			// Spawn actor
			if (GetOwner()->HasAuthority())
			{
				FTransform ControlledPawnTransform{FTransform()};
				if (const APawn* ControlledPawn{Cast<AController>(GetOwner())->GetPawn()})
				{
					ControlledPawnTransform = ControlledPawn->GetActorTransform();
				}

				// Spawn actor
				const FVector ForwardPawnDir{ControlledPawnTransform.GetRotation().GetForwardVector()},
				SpawnedLoc{ControlledPawnTransform.GetLocation() + FVector(ForwardPawnDir * 90.0)},
				ImpulseLoc{ControlledPawnTransform.GetLocation() + FVector(ForwardPawnDir * 75.0)};
						
				if(AGmRIS_WorldCollectableSM* NewSpawnedItem{
					GetWorld()->SpawnActor<AGmRIS_WorldCollectableSM>(AGmRIS_WorldCollectableSM::StaticClass(), SpawnedLoc,
						FRotator(ControlledPawnTransform.GetRotation()), FActorSpawnParameters())})
				{
					// SetMesh
					NewSpawnedItem->SetMesh(ConsumableInfo->MeshToBeDropped);
						
					// Add item definition
					//@TODO The quantity of items to be dropped may differ from the initial value, so do not use the AddStaticItem() function?
					// NewSpawnedItem->AddStaticItem(InUsedItemInst.ItemInst->GetItemDefinition());
					NewSpawnedItem->AddStaticItemFromInst(InUsedItemInst.ItemInst);

					// Add radial impulse
					if (UPrimitiveComponent* ItemMeshComp{Cast<UPrimitiveComponent>(NewSpawnedItem->GetMeshComp())})
					{
						ItemMeshComp->AddRadialImpulse(ImpulseLoc, 150.0, 8000.0,RIF_Linear,
							false);
					}
				}
			}

			// Internally, completely remove item instances from the inventory manager component.
			RemoveItemInstance(InUsedItemInst.ItemInst);
		}
		else
		{
			return;
		}
		UGameplayMsgSubsystem::Get(this).BroadcastMessage(
			GmGameplayTags::TAG_GmRIS_QuickBar_Msg_SlotItemSynchronization, InUsedItemInst);
	}//~End of consumable.

	#if !UE_BUILD_SHIPPING
	//@TODO Debug-only option that should be removed before release.
	TArray Loc_CurrentAllItems{GetAllItems()};
	for (int32 i{0}; i < Loc_CurrentAllItems.Num(); i++)
	{
		if (Loc_CurrentAllItems.IsValidIndex(i))
		{
			if (const UGmRIS_ItemFragment_InventoryTile* ItemTileInfo{
				Loc_CurrentAllItems[i]->FindFragmentByClass<UGmRIS_ItemFragment_InventoryTile>()})
			{
				UE_CLOG(bIsDebug, LogTemp, Error, L"Current %ist item : %s, count is %i.", i,
					*ItemTileInfo->DisplayNameWhenAcquired.ToString(),
					Loc_CurrentAllItems[i]->GetStatTagStackCount(GmGameplayTags::TAG_GmRIS_Inventory_Item_Quantity));
			}
		}
	}
#endif
}

void UGmRIS_InventoryManagerComponent::Internal_ClientNotifyAddedEvent(
	const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InAddedCount) const
{
	if (!InNotifyFragmentInfo)
	{
		UE_CLOG(bIsDebug, LogTemp, Error,
			L"UGmRIS_InventoryManagerComponent : Internal_ClientNotifyAddedEvent() - InNotifyFragmentInfo is nullptr");
	}
	if (GmMainLayoutWidgetCache)
	{
		GmMainLayoutWidgetCache->GmOnItemAdded(
			Cast<UGmRIS_ItemFragment_NotificationInfo>(InNotifyFragmentInfo), InAddedCount);
	}
}

void UGmRIS_InventoryManagerComponent::Internal_ClientNotifyRemovedEvent(
	const UGmRIS_InventoryItemFragment* InNotifyFragmentInfo, const int32 InRemovedCount) const
{
	if (!InNotifyFragmentInfo)
	{
		UE_CLOG(bIsDebug, LogTemp, Error,
			L"UGmRIS_InventoryManagerComponent : Internal_ClientNotifyAddedEvent() - InNotifyFragmentInfo is nullptr");
	}
	if (GmMainLayoutWidgetCache)
	{
		GmMainLayoutWidgetCache->GmOnItemRemoved(
			Cast<UGmRIS_ItemFragment_NotificationInfo>(InNotifyFragmentInfo), InRemovedCount);
	}
}
