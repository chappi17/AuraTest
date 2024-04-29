// Copyright WhNi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
//인터페이스 클래스는 AbilitySystemComponent 의 getter를 가지고 있음 ( 순수 가상함수 형태로)
// 그리고 AI와 플레이어 둘다 필요한 부분이지만, AI는 그 클래스에서 구현하고, Player는 State에서 할당하기 때문에 따로 정의함.
class AURA_API AAuraCharacterBase : public ACharacter , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UAttributeSet* GetAttributeSet()  const { return AttributeSet; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	// 캐릭터들 모두 GAS의 필수 요소를 가지고 있어야함
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();


};
