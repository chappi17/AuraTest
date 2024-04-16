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

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	// 블루프린트에서 내용을 재정의할 수 있게 하는 키워드
	UFUNCTION(BlueprintImplementableEvent)
	void  WidgetControllerSet();
	
};
