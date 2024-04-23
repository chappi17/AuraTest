// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CommonTileView.h"

#include "GmRISW_EnhancedTileView.generated.h"

class UGmRIS_InventoryItemInstance;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRISW_EnhancedTileView : public UCommonTileView
{
	GENERATED_BODY()

public:

	UGmRISW_EnhancedTileView(const FObjectInitializer& ObjectInitializer);

	// FGmOnSelectedItem OnClickedAnyItem;

	//~UListView Interface
	// virtual FOnItemSelectionChanged& OnItemSelectionChanged() const override;
	//~End of UListView Interface

	virtual void OnItemClickedInternal(UObject* Item) override;

	UFUNCTION()
	void SwapListItemsByIndex(const int64 InTargetA, const int64 InTargetB);

	UFUNCTION()
	void SwapListItemsByPtr(UObject* InTargetA, UObject* InTargetB);

	void SwapListItemsByEntryWidget(const IUserObjectListEntry* InEntryWidgetA, const IUserObjectListEntry* InEntryWidgetB);

protected:
	
	//~UListView Interface
	virtual void OnItemsChanged(const TArray<UObject*>& AddedItems, const TArray<UObject*>& RemovedItems) override;
	//~End of UListView Interface
	
};
