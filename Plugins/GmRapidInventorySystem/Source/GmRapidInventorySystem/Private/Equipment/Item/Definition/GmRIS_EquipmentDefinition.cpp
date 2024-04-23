// Copyright 2024 Dev.GaeMyo. All Rights Reserved.


#include "Equipment/Item/Definition/GmRIS_EquipmentDefinition.h"

#include "Equipment/GmRIS_EquipmentInstance.h"

UGmRIS_EquipmentDefinition::UGmRIS_EquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	InstanceType = UGmRIS_EquipmentInstance::StaticClass();
}
