// Copyright WhNi

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

class UAuraAttributeSet;
class UAttributeSet;

// 구조체 형태로 컨트롤러, 스테이트, 어빌리티시스템,어트리뷰트 연결하기 쉽도록 만들기
// UI에 영향주는 매개변수들 모은거임.
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
	:PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;

};


UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
// Widget Controller 는 위젯과 데이터를 연결해주는 백엔드라고 생각하면 될듯
// 데이터와 위젯을 연결하는게 중요할듯
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();

protected:
	// Widget 에 영향을 주는 (수치를 조절하는) 클래스들 담기
	UPROPERTY(BlueprintReadOnly,  Category ="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly,  Category ="WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly,  Category ="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly,  Category ="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
};
