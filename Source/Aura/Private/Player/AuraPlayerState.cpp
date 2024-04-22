// Copyright WhNi

#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	// Mixed, Minimal, Full -> 
	// Full -> 객체의 모든 속성이 서버에서 모든 클라이언트로 복제 (싱글 플레이 게임)
	// Mixed -> 객체의 일부 속성만 선택적으로 복사. (주로 멀티플레이어), 게임이펙트 또한 소유된 클라이언트에게만 복제
	// Minimal -> 필수적인 최소한의 정보만 복제 ( 주로 AI), 이펙트는 복제 안됨. 그래도 큐랑 태그는 복제됨.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("ArrtibuteSet");
	// 서버가 클라이언트를 업데이트 하는 주기를 뜻함
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
