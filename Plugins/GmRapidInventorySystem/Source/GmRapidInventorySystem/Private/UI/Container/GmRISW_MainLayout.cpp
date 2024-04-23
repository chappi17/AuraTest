// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "UI/Container/GmRISW_MainLayout.h"

#include <CommonHierarchicalScrollBox.h>
#include <Runtime/Launch/Resources/Version.h>

#include "Inventory/Item/Fragments/GmRIS_ItemFragment_InventoryTile.h"
#include "Inventory/Item/Fragments/GmRIS_ItemFragment_NotificationInfo.h"
#include "UI/Widgets/Notification/GmRISW_Notification.h"

UGmRISW_MainLayout::UGmRISW_MainLayout(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UGmRISW_MainLayout::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Init static widgets setting from runtime.
#if ENGINE_MAJOR_VERSION == 5
#if ENGINE_MINOR_VERSION <= 1
	NotificationScrollBox->WidgetStyle.TopShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->WidgetStyle.BottomShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->WidgetStyle.LeftShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->WidgetStyle.RightShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->bAllowRightClickDragScrolling = false;
#else
	FScrollBoxStyle NewNotifyScrollBoxStyle;
	NewNotifyScrollBoxStyle.TopShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NewNotifyScrollBoxStyle.BottomShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NewNotifyScrollBoxStyle.LeftShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NewNotifyScrollBoxStyle.RightShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->SetWidgetStyle(NewNotifyScrollBoxStyle);
	NotificationScrollBox->SetAllowRightClickDragScrolling(false);
#endif
#endif

	NotificationScrollBox->SetScrollBarVisibility(ESlateVisibility::Collapsed);
}

void UGmRISW_MainLayout::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Init static widgets setting from editor time.
#if ENGINE_MAJOR_VERSION == 5
#if ENGINE_MINOR_VERSION <= 1
	NotificationScrollBox->WidgetStyle.TopShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->WidgetStyle.BottomShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->WidgetStyle.LeftShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->WidgetStyle.RightShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->bAllowRightClickDragScrolling = false;
#else
	FScrollBoxStyle NewNotifyScrollBoxStyle;
	NewNotifyScrollBoxStyle.TopShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NewNotifyScrollBoxStyle.BottomShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NewNotifyScrollBoxStyle.LeftShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NewNotifyScrollBoxStyle.RightShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	NotificationScrollBox->SetWidgetStyle(NewNotifyScrollBoxStyle);
	NotificationScrollBox->SetAllowRightClickDragScrolling(false);
#endif
#endif
}

void UGmRISW_MainLayout::GmOnItemAdded(const UGmRIS_ItemFragment_NotificationInfo* InNotifyFragmentInfo,
	const int32 InAddedCount) const
{
	// UE_LOG(LogTemp, Error, L"MainLayout - GmOnItemAdded()");

	if (!InNotifyFragmentInfo)
	{
		UE_LOG(LogTemp, Error, L"MainLayout - GmOnItemAdded() : InNotifyFragmentInfo is not valid.");
		return;
	}
	
	if (NotifyWidgetClass.IsValid())
	{
		if (UGmRISW_Notification* NewNotifyWidget{
			CreateWidget<UGmRISW_Notification>(GetOwningPlayer(),
				NotifyWidgetClass.TryLoadClass<UGmRISW_Notification>())})
		{
			NewNotifyWidget->FadeOutDelay = InNotifyFragmentInfo->NotificationAddedInfo.FadeOutDelay;
			NewNotifyWidget->Forward_Reverse_WidgetAnimPlayRate = FVector2D(
				InNotifyFragmentInfo->NotificationAddedInfo.AppearanceWidgetAnimPlayRate,
				InNotifyFragmentInfo->NotificationAddedInfo.DisappearanceWidgetAnimPlayRate);
			NewNotifyWidget->NotificationTitleText = InNotifyFragmentInfo->NotificationAddedInfo.NotifyName;
			NewNotifyWidget->NotificationContentText = InNotifyFragmentInfo->NotificationAddedInfo.NotificationTargetName;
			NewNotifyWidget->NotificationsStackCount = InAddedCount;
			NewNotifyWidget->NotificationIconBrush.SetResourceObject(InNotifyFragmentInfo->NotificationAddedInfo.NotifyIconObj);
			
			NotificationScrollBox->AddChild(NewNotifyWidget);
		}
	}
}

void UGmRISW_MainLayout::GmOnItemRemoved(const UGmRIS_ItemFragment_NotificationInfo* InNotifyFragmentInfo,
	const int32 InRemovedCount) const
{
	// UE_LOG(LogTemp, Error, L"MainLayout - GmOnItemRemoved()");

	if (!InNotifyFragmentInfo)
	{
		UE_LOG(LogTemp, Error, L"MainLayout - GmOnItemRemoved() : InNotifyFragmentInfo is not valid.");
		return;
	}
	
	if (NotifyWidgetClass.IsValid())
	{
		if (UGmRISW_Notification* NewNotifyWidget{
			CreateWidget<UGmRISW_Notification>(GetOwningPlayer(),
				NotifyWidgetClass.TryLoadClass<UGmRISW_Notification>())})
		{
			NewNotifyWidget->FadeOutDelay = InNotifyFragmentInfo->NotificationRemovedInfo.FadeOutDelay;
			NewNotifyWidget->Forward_Reverse_WidgetAnimPlayRate = FVector2D(
				InNotifyFragmentInfo->NotificationRemovedInfo.AppearanceWidgetAnimPlayRate,
				InNotifyFragmentInfo->NotificationRemovedInfo.DisappearanceWidgetAnimPlayRate);
			NewNotifyWidget->NotificationTitleText = InNotifyFragmentInfo->NotificationRemovedInfo.NotifyName;
			NewNotifyWidget->NotificationContentText = InNotifyFragmentInfo->NotificationRemovedInfo.NotificationTargetName;
			NewNotifyWidget->NotificationsStackCount = InRemovedCount;
			NewNotifyWidget->NotificationIconBrush.SetResourceObject(InNotifyFragmentInfo->NotificationRemovedInfo.NotifyIconObj);
			
			NotificationScrollBox->AddChild(NewNotifyWidget);
		}
	}
}

// void UGmRISW_MainLayout::HandleEscapeAction()
// {
	// if (ensure(!EscapeMenuClass.IsNull()))
	// {
		// UGmAdvancedUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_MENU, EscapeMenuClass);
	// }
// }

#undef LOCTEXT_NAMESPACE
