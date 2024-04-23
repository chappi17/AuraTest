// Copyright 2024 Dev.GaeMyo. All Rights Reserved.


#include "Equipment/GmRIS_EquipmentInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Equipment/Item/Definition/GmRIS_EquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

class FLifetimeProperty;
class UClass;
class USceneComponent;

UGmRIS_EquipmentInstance::UGmRIS_EquipmentInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGmRIS_EquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

#if UE_WITH_IRIS
void UGmRIS_EquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	Super::RegisterReplicationFragments(Context, RegistrationFlags);

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

UWorld* UGmRIS_EquipmentInstance::GetWorld() const
{
	if (const APawn* Loc_OwningPawn{GetPawn()})
	{
		return Loc_OwningPawn->GetWorld();
	}
	return nullptr;
}

APawn* UGmRIS_EquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UGmRIS_EquipmentInstance::GetTypedPawn(const TSubclassOf<APawn> InPawnType) const
{
	APawn* Result{nullptr};
	if (UClass* ActualPawnType{InPawnType})
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void UGmRIS_EquipmentInstance::SpawnEquipmentActors(const TArray<FGmRISEquipmentActorToSpawn>& InActorsToSpawn)
{
	if (APawn* OwningPawn{GetPawn()})
	{
		USceneComponent* AttachTarget{OwningPawn->GetRootComponent()};
		if (const ACharacter* Char{Cast<ACharacter>(OwningPawn)})
		{
			AttachTarget = Char->GetMesh();
		}

		for (const FGmRISEquipmentActorToSpawn& SpawnInfo : InActorsToSpawn)
		{
			AActor* NewActor{GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn)};
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void UGmRIS_EquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Loc_Actor : SpawnedActors)
	{
		if (!Loc_Actor)
		{
			continue;
		}
		Loc_Actor->Destroy();
	}
}

void UGmRIS_EquipmentInstance::OnEquipped()
{
	Gb2_OnEquipped();
}

void UGmRIS_EquipmentInstance::OnUnequipped()
{
	Gb2_OnUnequipped();
}

void UGmRIS_EquipmentInstance::OnRep_Instigator()
{
}
