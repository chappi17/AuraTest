// Copyright WhNi

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
// 직접적으로 소유권을 가지는게 아니라, AbilitySystem에 의해서 State에서 할당받음. 그래서 중요 구현은 State에서 일어남.
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();

	// 액터에 대한 소유권을 갖는 함수
	virtual void PossessedBy(AController* NewController) override;
	// SetPlayerState 세팅
	virtual void OnRep_PlayerState() override;
	// 액터에 대한 초기화 하는 함수

	virtual int32 GetPlayerLevel() override;


private:
	virtual void InitAbilityActorInfo() override;

};
