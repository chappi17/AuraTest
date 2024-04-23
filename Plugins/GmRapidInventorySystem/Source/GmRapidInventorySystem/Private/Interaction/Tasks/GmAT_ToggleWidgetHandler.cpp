// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "Interaction/Tasks/GmAT_ToggleWidgetHandler.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Global/GmRIS_GlobalFuncLib.h"
#include "Inventory/GmRIS_InventoryManagerComponent.h"
#include "UI/Container/GmRISW_InventoryContainer.h"

UGmAT_ToggleWidgetHandler::UGmAT_ToggleWidgetHandler(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UGmAT_ToggleWidgetHandler::Activate()
{
	SetWaitingOnAvatar();
	
	GmUpdateOrToggleWidget();
}

UGmAT_ToggleWidgetHandler* UGmAT_ToggleWidgetHandler::ToggleWidgetHandler(UGameplayAbility* InOwningAbility,
const TSubclassOf<UGmRISW_InventoryContainer> InInventoryWidgetClassRef)
{
	UE_CLOG(!InInventoryWidgetClassRef, LogTemp, Error, L"Inventory Widget Class Reference is NULL.");
	
	// UAbilityTask_WaitInputPress* Loc_NewInputPressed{NewAbilityTask<UAbilityTask_WaitInputPress>(InOwningAbility)};
	UGmAT_ToggleWidgetHandler* Loc_MyObj{NewAbilityTask<UGmAT_ToggleWidgetHandler>(InOwningAbility)};
	
	Loc_MyObj->InventoryContainerWidgetClassRef = InInventoryWidgetClassRef;
	
	// Loc_NewInputPressed->OnPress.AddUniqueDynamic(Loc_MyObj, &UGmAT_ToggleWidgetHandler::GmOnToggleInputPressed);

	return Loc_MyObj;
}

void UGmAT_ToggleWidgetHandler::OnDestroy(const bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

// void UGmAT_ToggleWidgetHandler::GmOnToggleInputPressed(float InTimeWaited)
// {
// }

void UGmAT_ToggleWidgetHandler::GmUpdateOrToggleWidget() const
{
	if (const APawn* Loc_AvatarPawn{Cast<APawn>(Ability->GetCurrentActorInfo()->AvatarActor)})
	{
		if (UGmRIS_InventoryManagerComponent* Loc_CurrentInventoryManager{
			Loc_AvatarPawn->GetController()->FindComponentByClass<UGmRIS_InventoryManagerComponent>()})
		{
			if (Loc_CurrentInventoryManager->GmGetInventoryMainWidget())
			{
				Loc_CurrentInventoryManager->GmSetInventoryMainWidget(nullptr);
				RemovedInventoryWidget.Broadcast(nullptr);
			}
			else
			{
				UE_CLOG(!InventoryContainerWidgetClassRef, LogTemp, Error, L"Inventory Widget Class Reference is NULL.");
				if (InventoryContainerWidgetClassRef)
				{
					UGmRISW_InventoryContainer* Loc_NewInventoryMainWidget{CreateWidget<UGmRISW_InventoryContainer>(
					UGmRIS_GlobalFuncLib::GetPCFromSpecificIndex(this, 0), InventoryContainerWidgetClassRef)};

					//@TODO Handling dependencies from "UWorld" for generated widgets.
					Loc_CurrentInventoryManager->GmSetInventoryMainWidget(Loc_NewInventoryMainWidget);
					
					const FGmCreatedInventoryWidgetInfo Loc_NewCreatedInventoryWidgetInfo{FGmCreatedInventoryWidgetInfo(Loc_NewInventoryMainWidget)};
					AfterCreatedInventoryWidget.Broadcast(Loc_NewCreatedInventoryWidgetInfo);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, L"Ability avatar actor is not valid.");
	}
}
