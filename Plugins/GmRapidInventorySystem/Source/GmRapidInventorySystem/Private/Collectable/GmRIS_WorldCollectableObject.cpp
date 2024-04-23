// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "Collectable/GmRIS_WorldCollectableObject.h"

#include <AbilitySystemGlobals.h>
#include <GameplayCueManager.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/StaticMesh.h>

#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"
#include "UObject/ConstructorHelpers.h"

FName AGmRIS_WorldCollectableObject::MeshComponentName{L"Default Mesh"};

struct FGmRIS_InteractionQuery;

// Sets default values
AGmRIS_WorldCollectableObject::AGmRIS_WorldCollectableObject(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
    bAlwaysRelevant = true;
	
    ThisClass::SetReplicateMovement(true);
	
	if (MeshComponent)
	{
		MeshComponent->SetEnableGravity(false);
		MeshComponent->SetIsReplicated(true);
	}

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->AddGameplayCueNotifyPath(L"/GmRapidInventorySystem/GameplayCues");

	if (!InteractionOpt.InteractionAbilityToGrant)
	{
		struct FConstructorStatics
		{
			ConstructorHelpers::FClassFinder<UGameplayAbility> Interact_CollectableAbility;
			FConstructorStatics()
				:
			Interact_CollectableAbility(L"/Script/Engine.Blueprint'/GmRapidInventorySystem/Demo/Abilities/GA_Interact_Collect'")
			{}
		};

		static FConstructorStatics GmS_Helper;
		if (GmS_Helper.Interact_CollectableAbility.Succeeded())
		{
			InteractionOpt.InteractionAbilityToGrant = GmS_Helper.Interact_CollectableAbility.Class;
		}
	}
}

void AGmRIS_WorldCollectableObject::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetMesh(Item_WorldMesh);
}

// void AGmRIS_WorldCollectableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// }
//
// bool AGmRIS_WorldCollectableObject::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
// 	FReplicationFlags* RepFlags)
// {
// 	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
// }

void AGmRIS_WorldCollectableObject::GatherInteractionOptions(const FGmRIS_InteractionQuery& InteractQuery,
	FGmRIS_InteractionOptBuilder& InInteractionBuilder)
{
	InInteractionBuilder.AddInteractionOption(InteractionOpt);
}

FGmRIS_InventoryPickup AGmRIS_WorldCollectableObject::GetPickupInventory() const
{
	UE_CLOG(bIsDebugging, LogTemp, Error, L"%s : GetPickupInventory()", *GetNameSafe(this));
	return InventoryStatic;
}

void AGmRIS_WorldCollectableObject::SetMesh(UStreamableRenderAsset* InNewMesh)
{
	Item_WorldMesh = InNewMesh;
}

void AGmRIS_WorldCollectableObject::AddStaticItem(const TSubclassOf<UGmRIS_InventoryItemDefinition> InAdditionalItemDef)
{
	if(!InAdditionalItemDef) return;
	
	InventoryStatic.Templates.Add(FGmRIS_PickupTemplate(InAdditionalItemDef));
}

void AGmRIS_WorldCollectableObject::AddStaticItemFromInst(UGmRIS_InventoryItemInstance* InItemInst)
{
	if(!InItemInst) return;
	
	InventoryStatic.Instances.Add(FGmRIS_PickupInstance(InItemInst));
}
