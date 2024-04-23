// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Inventory/Item/Definition/GmRIS_InventoryItemDefinition.h"

#include "GmRIS_ItemFragment_NotificationInfo.generated.h"

UENUM(BlueprintType)
enum class EGmNotificationType : uint8
{
	EOnlyAdded = 0 UMETA(DisplayName = "Only Added"),
	EOnlyRemoved = 1 UMETA(DisplayName = "Only Removed"),
	BothAddedAndRemoved = 2 UMETA(DisplayName = "Both Added and Removed")
};

USTRUCT(BlueprintType)
struct FGmNotificationInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info", meta = (ClampMin = 1.f, ClampMax = 5.f))
	float FadeOutDelay{2.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info", meta = (ClampMin = 0.1f, ClampMax = 3.f), DisplayName = "Notification appearance anim Playrate")
	float AppearanceWidgetAnimPlayRate{1.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info", meta = (ClampMin = 0.1f, ClampMax = 3.f), DisplayName = "Notification disappearance anim Playrate")
	float DisappearanceWidgetAnimPlayRate{1.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info")
	FText NotifyName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info")
	FText NotificationTargetName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info", meta = (DisplayThumbnail = true, DisplayName = "Notify Icon Object", AllowedClasses = "/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"))
	TObjectPtr<UObject> NotifyIconObj;
	
};

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRIS_ItemFragment_NotificationInfo : public UGmRIS_InventoryItemFragment
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info")
	EGmNotificationType NotificationBroadcastType;

	// Notification added info
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info|Added", meta = (EditCondition = "NotificationBroadcastType != EGmNotificationType::EOnlyRemoved"))
	FGmNotificationInfo NotificationAddedInfo;

	// Notification removed info
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification Info|Removed", meta = (EditCondition = "NotificationBroadcastType != EGmNotificationType::EOnlyAdded"))
	FGmNotificationInfo NotificationRemovedInfo;
	
};
