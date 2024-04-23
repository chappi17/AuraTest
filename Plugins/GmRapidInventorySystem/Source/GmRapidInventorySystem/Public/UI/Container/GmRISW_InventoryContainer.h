// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CommonActivatableWidget.h"
#include "GmRISW_InventoryContainer.generated.h"

struct FGmSimpleItemInstance;
struct FGameplayTag;
class UButton;
class UCommonRichTextBlock;
class UCommonLazyImage;
class UGmRIS_InventoryItemInstance;
class UGmRIS_ItemFragment_InventoryTile;
class UGmRISW_InventoryPanel;
class UWidgetAnimation;

USTRUCT(BlueprintType)
struct FGmMainInventoryInitSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings")
	int32 InventoryGridQuantity{35};
	
};

USTRUCT(BlueprintType)
struct FItemDescriptionSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings")
	float AppearanceAnimPlaybackMultiplier{2.f};
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings")
	float DisappearanceAnimPlaybackMultiplier{2.f};
	
};

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRISW_InventoryContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings")
	bool bIsDebug{false};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings")
	FItemDescriptionSettings ItemDescriptionSettings;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings")
	FGmMainInventoryInitSettings MainInventoryInitSettings;

	UPROPERTY(meta = (BindWidget/*, AllowPrivateAccess*/), BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UGmRISW_InventoryPanel> GmSimpleInventoryGrid;
	
protected:
	
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess), BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UCommonLazyImage> ItemDescriptionImage;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess), BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UCommonRichTextBlock> ItemLongDescriptionText;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess), BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UButton> UsingItemBtn;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess), BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UButton> DropItemBtn;

	// Widget Animations.
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ItemInfoAppearanceAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> InventoryAppearance;
	
	//~UUser Widget Interface
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUser Widget Interface
	
private:
	
	UFUNCTION()
	void UseAnItem();

	// Use or drop
	void UpdateMainInventory(FGameplayTag InTargetTag, const FGmSimpleItemInstance& InTargetItemInst);

	UFUNCTION()
	void DropsAnItem();

	UFUNCTION()
	void RefreshFirstItemExistsOrNot() const;

	//~ Utils
	void GmSetItemDescImgOpacity(bool InIsOn) const;

};
