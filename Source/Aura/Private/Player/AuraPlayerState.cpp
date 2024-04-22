// Copyright WhNi

#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	// Mixed, Minimal, Full -> 
	// Full -> ��ü�� ��� �Ӽ��� �������� ��� Ŭ���̾�Ʈ�� ���� (�̱� �÷��� ����)
	// Mixed -> ��ü�� �Ϻ� �Ӽ��� ���������� ����. (�ַ� ��Ƽ�÷��̾�), ��������Ʈ ���� ������ Ŭ���̾�Ʈ���Ը� ����
	// Minimal -> �ʼ����� �ּ����� ������ ���� ( �ַ� AI), ����Ʈ�� ���� �ȵ�. �׷��� ť�� �±״� ������.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("ArrtibuteSet");
	// ������ Ŭ���̾�Ʈ�� ������Ʈ �ϴ� �ֱ⸦ ����
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
