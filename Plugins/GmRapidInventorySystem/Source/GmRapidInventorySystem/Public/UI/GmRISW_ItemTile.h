// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMsgSubsystem.h"

#include "GmRISW_ItemTile.generated.h"

struct FGmSimpleItemInstance;
struct FGmRISInventoryChangeMsg;
DECLARE_DYNAMIC_DELEGATE_OneParam(FGmOnNewDroppedItem, const UGmRISW_ItemTile*, InTargetItemTile);

class UGmRIS_ItemFragment_InventoryTile;
class UGmRIS_InventoryItemInstance;
class UCommonBorder;
class UTextBlock;
class UCommonTextBlock;
class UCommonLazyImage;

USTRUCT(BlueprintType)
struct FGmTileViewSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings|Item Tile Unit", DisplayName = "Default Item Background Color")
	FLinearColor DefaultItemBGColor{FLinearColor(0.f, 0.f, 0.f, 0.3f)};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings|Item Tile Unit", DisplayName = "Item Background Color When Selected")
	FLinearColor ItemBGColorWhenSelected{FLinearColor(0.f, 0.f, 0.f, 0.3f)};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings|Item Tile Unit")
	FLinearColor SelectedOutlineColor{FLinearColor(1.f, 1.f, 0.15f, 0.8f)};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings|Item Tile Unit")
	FVector4 SelectedOutlineCornerRadius{FVector4(0.5f, 0.5f, 0.5f, 0.5f)};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Setting|Item Tile Unit")
	float SelectedOutlineWidth{4.f};
	
};

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRISW_ItemTile : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> ItemLazyImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ItemSimpleName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonBorder> UnitMainBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemQuantity;

	//~IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	//~End of IUserObjectListEntry Interface

	//~ Utils
	void GmOnSelectedUnitMainBorder(bool InIsSelected) const;

	UFUNCTION(BlueprintCallable, Category = "GmRIS|Item Tile")
	void GmSetItemTileSettings(FGmTileViewSettings InNewItemTileSettings);

	void GmRefreshItemTileInfo();

	bool GmSetItemInst(UGmRIS_InventoryItemInstance* InNewItemInst);

	FORCEINLINE bool GmIsReady() const
	{
		return ItemInst && ItemInventoryTileFragment;
	}

	FORCEINLINE const UGmRIS_ItemFragment_InventoryTile* GmGetInventoryTileFragment() const
	{
		return ItemInventoryTileFragment;
	}

protected:
	
	//~UUser Widget Interface
	// virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
#if WITH_EDITOR
	virtual void NativePreConstruct() override;
#endif
	//~End of UUser Widget Interface

private:
	
	FGmTileViewSettings TileViewSelectedSettings;
	
	UPROPERTY()
	TObjectPtr<UGmRIS_InventoryItemInstance> ItemInst;

	UPROPERTY()
	TObjectPtr<const UGmRIS_ItemFragment_InventoryTile> ItemInventoryTileFragment;
	
	UFUNCTION()
	void SynchronizeSelectedItem(FGameplayTag InTargetTag, const FGmSimpleItemInstance& InChangedInfo);
	
};
