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
// Player���� �Ҵ�Ǵ� ���̱⿡ �����͸� ������ �־�� ��. �׷��� �������̽� �������ְ� ���� �ʱ�ȭ �������.
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	AAuraPlayerState();
	// ���������� ������Ʈ�� ���� ���� getter�� ������ �־�� ��.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UAttributeSet* GetAttributeSet()  const { return AttributeSet; }

protected:

	// �� Ŭ������ ���ؼ� ĳ������ ������ ������ �������.
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

};
