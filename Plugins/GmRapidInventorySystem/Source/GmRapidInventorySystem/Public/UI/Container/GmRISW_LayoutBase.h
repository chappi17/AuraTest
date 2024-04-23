// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CommonActivatableWidget.h"

#include "GmRISW_LayoutBase.generated.h"

/**
 *	UGmRISW_LayoutBase
 *
 *  Widget used to lay out the player's HUD.
 *  @TODO It will be separated into the "GmAdvancedUI" plugin later. - by Dev.GaeMyo
 */
UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRISW_LayoutBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	UGmRISW_LayoutBase(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:

	// Properties
	
	// void HandleEscapeAction();

	//~ UUserWidget Interface.
	virtual void NativeConstruct() override{Super::NativeConstruct();}
	virtual void NativePreConstruct() override{Super::NativePreConstruct();}
	//~End of UUserWidget Interface.

	// UPROPERTY(EditDefaultsOnly)
	// TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
	
// private:
	

};
