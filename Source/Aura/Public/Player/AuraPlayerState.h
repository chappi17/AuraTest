// Copyright WhNi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

/**
 *
 */
UCLASS()
// Player에게 할당되는 것이기에 포인터를 가지고 있어야 함. 그래서 인터페이스 세팅해주고 생성 초기화 해줘야함.
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	AAuraPlayerState();
	// 마찬가지로 컴포넌트와 셋의 대한 getter를 가지고 있어야 함.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UAttributeSet* GetAttributeSet()  const { return AttributeSet; }

protected:

	// 이 클래스를 통해서 캐릭터의 포인터 연결을 해줘야함.
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

};
