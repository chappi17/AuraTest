// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMsgSubsystem.h"

#include "UI/CWidgets/GmTaggedWidget.h"

#include "GmCW_QuickBarSlot.generated.h"

class UInputAction;
class UCommonNumericTextBlock;
struct FGmRISQuickBarSlotsChangedMsg;
struct FGmRISQuickBarActiveIndexChangedMsg;
class UGmRIS_QuickBarComponent;
class UImage;
class UGmRIS_InventoryItemInstance;
class APlayerController;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmCW_QuickBarSlot : public UGmTaggedWidget
{
	GENERATED_BODY()

public:

	//~UUser Widget Interface
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~End of UUser Widget Interface

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InitSettings)
	FName ItemCardTextureParamName{"IconTexture"};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InitSettings)
	FName ItemSubMiniIconTextureParamName{"TextureMask"};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = InitSettings)
	int SlotIndex;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = InitSettings)
	TObjectPtr<UInputAction> SlotInputAction;

protected:

	// Variables
	UPROPERTY(BlueprintReadOnly, Category = QuickSlotState)
	bool bIsEmpty{false};

	UPROPERTY(BlueprintReadOnly, Category = QuickSlotState)
	bool bIsSelected{false};


	// Properties
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemCard;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SubMiniIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonNumericTextBlock> ItemQuantity;

	// Widget Animations
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ActiveToInactive;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> InactiveToActive;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ToEmpty;

private:

	UFUNCTION(BlueprintPure = false, Category = GmUtils)
	bool IsThisTheActiveQuickBarSlot() const;

	void GmSimpleUpdateSlot(const UGmRIS_InventoryItemInstance* InItemInst);
	void UpdateIsSelected();
	void UpdateComponentRef(UGmRIS_QuickBarComponent* InQuickBarComponent);
	void UpdateItemQuantityLeft(const UGmRIS_QuickBarComponent* InQuickBarComponent) const;

	UPROPERTY()
	TWeakObjectPtr<UGmRIS_QuickBarComponent> QuickBarComponentWPtr;

	FGameplayMsgListenerHandle ActiveIndexChangedHandle;
	FGameplayMsgListenerHandle QuickBarSlotsChangedHandle;

	UFUNCTION()
	void GmQuickBarActiveIndexChangedFunc(FGameplayTag InChannel, const FGmRISQuickBarActiveIndexChangedMsg& InPayload);

	UFUNCTION()
	void GmQuickBarSlotsChangedFunc(FGameplayTag InChannel, const FGmRISQuickBarSlotsChangedMsg& InPayload);

};
