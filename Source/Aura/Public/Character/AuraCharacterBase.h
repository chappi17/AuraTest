// Copyright WhNi

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
//�������̽� Ŭ������ AbilitySystemComponent �� getter�� ������ ���� ( ���� �����Լ� ���·�)
// �׸��� AI�� �÷��̾� �Ѵ� �ʿ��� �κ�������, AI�� �� Ŭ�������� �����ϰ�, Player�� State���� �Ҵ��ϱ� ������ ���� ������.
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

	// ĳ���͵� ��� GAS�� �ʼ� ��Ҹ� ������ �־����
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();


};
