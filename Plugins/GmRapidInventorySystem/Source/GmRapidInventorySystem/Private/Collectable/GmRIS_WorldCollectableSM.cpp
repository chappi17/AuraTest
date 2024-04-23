// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "Collectable/GmRIS_WorldCollectableSM.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AGmRIS_WorldCollectableSM::AGmRIS_WorldCollectableSM(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(MeshComponentName))
{
	AGmRIS_WorldCollectableSM::GetMeshComp() = CreateOptionalDefaultSubobject<UStaticMeshComponent>(MeshComponentName);
	AGmRIS_WorldCollectableSM::GetMeshComp()->SetSimulatePhysics(true);
	AGmRIS_WorldCollectableSM::GetMeshComp()->CanCharacterStepUpOn = /*ECanBeCharacterBase::*/ECB_No;	

	RootComponent = AGmRIS_WorldCollectableSM::GetMeshComp();
	AGmRIS_WorldCollectableSM::GetMeshComp()->SetIsReplicated(true);
}

void AGmRIS_WorldCollectableSM::SetMesh(UStreamableRenderAsset* InNewMesh)
{
	Super::SetMesh(InNewMesh);
	
	Cast<UStaticMeshComponent>(GetMeshComp())->SetStaticMesh(Cast<UStaticMesh>(InNewMesh));
}
