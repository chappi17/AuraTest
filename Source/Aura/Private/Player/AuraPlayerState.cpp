// Copyright WhNi

#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AuraAbilitySystemComponent");
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

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	
}
