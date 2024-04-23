// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GmRISW_ItemTile.h"
#include "Blueprint/UserWidget.h"
// #include "GameFramework/GameplayMsgSubsystem.h"

#include "GmRISW_InventoryPanel.generated.h"

class UGmRIS_InventoryItemInstance;
class UGmRISW_EnhancedTileView;
struct FGmRISInventoryChangeMsg;
struct FGameplayTag;
class UGmRIS_InventoryManagerComponent;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRISW_InventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "Init Settings|Item Tile Unit")
	FGmTileViewSettings ItemTileViewSettings;
	
	UPROPERTY(EditAnywhere, Category = "Init Settings|Item Tile Unit")
	int32 InventoryGridQuantity{0};
	
	void ReGenerateItemTileView(TArray<UGmRIS_InventoryItemInstance*> InAllItems) const;

protected:
	
	friend class UGmRISW_InventoryContainer;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess), BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UGmRISW_EnhancedTileView> InventoryTileView;
	
	//~UUser Widget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUser Widget Interface

private:
	
};
