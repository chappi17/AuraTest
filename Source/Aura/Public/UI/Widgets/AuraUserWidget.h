// Copyright WhNi

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWIdgetController);

	// 위젯에서 컨트롤러로 정보를 줘야하기 때문에 필요
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	//In blueprint, editor can Implementation Event
	UFUNCTION(BlueprintImplementableEvent)
	void  WidgetControllerSet();
	
};
