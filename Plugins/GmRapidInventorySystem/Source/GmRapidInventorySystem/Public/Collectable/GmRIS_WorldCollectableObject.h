// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/Actor.h>

#include "Interaction/GmRIS_InteractionOpt.h"
#include "Interaction/IGmRIS_InteractableTarget.h"
#include "Interface/GmRISI_Pickable.h"

#include "GmRIS_WorldCollectableObject.generated.h"

class USkeletalMesh;
class UObject;
struct FGmRIS_InteractionQuery;
class UStreamableRenderAsset;

UCLASS(Abstract, Blueprintable)
class GMRAPIDINVENTORYSYSTEM_API AGmRIS_WorldCollectableObject : public AActor, public IGmRIS_InteractableTarget, public IGmRISI_Pickable
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMeshComponent> MeshComponent;
	
public:
	
	AGmRIS_WorldCollectableObject(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings|Debugging")
	bool bIsDebugging{false};

	//~AActor Interface
	virtual void OnConstruction(const FTransform& Transform) override;
	//~End of AActor Interface

	//~ActorReplication Interface
	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of ActorReplication Interface

	//~IGmRIS_InteractableTarget Interface
	virtual void GatherInteractionOptions(const FGmRIS_InteractionQuery& InteractQuery, FGmRIS_InteractionOptBuilder& InInteractionBuilder) override;
	virtual FGmRIS_InventoryPickup GetPickupInventory() const override;
	//~End of IGmRIS_InteractableTarget Interface

	virtual void SetMesh(UStreamableRenderAsset* InNewMesh);

	void AddStaticItem(TSubclassOf<UGmRIS_InventoryItemDefinition> InAdditionalItemDef);

	void AddStaticItemFromInst(UGmRIS_InventoryItemInstance* InItemInst);

protected:

	static FName MeshComponentName;

	virtual TObjectPtr<UMeshComponent>& GetMeshComp()
	{
		return MeshComponent;
	}

	UPROPERTY(EditAnywhere, DisplayName = "Interaction Options", Category = InitSettings)
	FGmRIS_InteractionOpt InteractionOpt;
	
	UPROPERTY(EditAnywhere, Category = InitSettings)
	FGmRIS_InventoryPickup InventoryStatic;

	UPROPERTY(EditAnywhere, Category = InitSettings)
	TObjectPtr<UStreamableRenderAsset> Item_WorldMesh{nullptr};

};
