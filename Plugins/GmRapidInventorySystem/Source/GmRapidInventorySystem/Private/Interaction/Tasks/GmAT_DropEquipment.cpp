// Copyright 2024 Dev.GaeMyo. All Rights Reserved.


#include "Interaction/Tasks/GmAT_DropEquipment.h"

#include <Engine/World.h>
#include <GameFramework/Controller.h>

#include "Equipment/GmRIS_QuickBarComponent.h"
#include "Global/GmRIS_GlobalFuncLib.h"
#include "Inventory/GmRIS_InventoryManagerComponent.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_Consumable.h"

UGmAT_DropEquipment::UGmAT_DropEquipment(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGmAT_DropEquipment::Activate()
{
	SetWaitingOnAvatar();

	GmDropEquipment();
}

UGmAT_DropEquipment* UGmAT_DropEquipment::DropEquipment(UGameplayAbility* InOwningAbility)
{
	UGmAT_DropEquipment* Loc_ThisObj{NewAbilityTask<UGmAT_DropEquipment>(InOwningAbility)};
	return Loc_ThisObj;
}

void UGmAT_DropEquipment::GmDropEquipment() const
{
	if (!Ability)
	{
		return;
	}
	if (Ability->GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority)
	{
		if (const AController* Loc_C{UGmRIS_GlobalFuncLib::GetControllerFromActorInfo(*Ability->GetCurrentActorInfo())})
		{
			if (UGmRIS_QuickBarComponent* Loc_QuickBarComponent{Loc_C->FindComponentByClass<UGmRIS_QuickBarComponent>()})
			{
				if (UGmRIS_InventoryItemInstance* TargetItemInst{
					Loc_QuickBarComponent->RemoveItemFromSlot(Loc_QuickBarComponent->GetActiveSlotIndex())})
				{
					if (UGmRIS_InventoryManagerComponent* InventoryManagerComp{
						Loc_C->FindComponentByClass<UGmRIS_InventoryManagerComponent>()})
					{
						InventoryManagerComp->ItemHasBeenUsed(FGameplayTag(),
							FGmSimpleItemInstance(TargetItemInst, Loc_C, true, true));
					}
				}
				
				//@TODO Need?
				// Loc_QuickBarComponent->CycleActiveSlotForward();
			}
		}
	}
	//@TODO End Ability();
}
