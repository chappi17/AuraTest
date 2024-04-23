// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GmRISW_LayoutBase.h"

#include "GmRISW_MainLayout.generated.h"

class UGmRIS_ItemFragment_NotificationInfo;
class UGmRIS_ItemFragment_InventoryTile;
class UCommonHierarchicalScrollBox;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRISW_MainLayout : public UGmRISW_LayoutBase
{
	GENERATED_BODY()

public:

	UGmRISW_MainLayout(const FObjectInitializer& ObjectInitializer);

	void GmOnItemAdded(const UGmRIS_ItemFragment_NotificationInfo* InNotifyFragmentInfo, const int32 InAddedCount) const;
	void GmOnItemRemoved(const UGmRIS_ItemFragment_NotificationInfo* InNotifyFragmentInfo, const int32 InRemovedCount) const;

protected:

	// Properties


	// Notify widget section.
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess), BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UCommonHierarchicalScrollBox> NotificationScrollBox;

	UPROPERTY(EditDefaultsOnly, Category = "Init Settings")
	FSoftClassPath NotifyWidgetClass;


	//~ UUserWidget Interface.
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	//~End of UUserWidget Interface.

};
