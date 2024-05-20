// Copyright WhNi

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UOverlayWidgetController;
class UAttributesMenuWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetConctroller(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "WidgetController")
	static UAttributesMenuWidgetController* GetAttributesMenuWidgetController(const UObject* WorldContextObject);
	
};
