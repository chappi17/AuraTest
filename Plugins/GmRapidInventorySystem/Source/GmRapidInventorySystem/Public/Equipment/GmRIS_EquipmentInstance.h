// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GmRIS_EquipmentInstance.generated.h"

class AActor;
class APawn;
struct FFrame;
struct FGmRISEquipmentActorToSpawn;

UCLASS(BlueprintType, Blueprintable)
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_EquipmentInstance : public UObject
{
	GENERATED_BODY()

public:

	UGmRIS_EquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;
	//~End of UObject interface

	UFUNCTION(BlueprintPure, Category = GmRISEquipment)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = GmRISEquipment, meta = (DeterminesOutputType = InPawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> InPawnType) const;

	UFUNCTION(BlueprintPure, Category = GmRISEquipment)
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	virtual void SpawnEquipmentActors(const TArray<FGmRISEquipmentActorToSpawn>& InActorsToSpawn);
	virtual void DestroyEquipmentActors();

	virtual void OnEquipped();
	virtual void OnUnequipped();

protected:

#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS
	
	UFUNCTION(BlueprintImplementableEvent, Category = GmRISEquipment, meta = (DisplayName = "OnEquipped"))
	void Gb2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = GmRISEquipment, meta = (DisplayName = "OnUnequipped"))
	void Gb2_OnUnequipped();

private:

	UFUNCTION()
	void OnRep_Instigator();

	UPROPERTY(ReplicatedUsing = OnRep_Instigator)
	TObjectPtr<UObject> Instigator;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;
	
};
