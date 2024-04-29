// Copyright WhNi

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
// ���������� �������� �����°� �ƴ϶�, AbilitySystem�� ���ؼ� State���� �Ҵ����. �׷��� �߿� ������ State���� �Ͼ.
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();

	// ���Ϳ� ���� �������� ���� �Լ�
	virtual void PossessedBy(AController* NewController) override;
	// SetPlayerState ����
	virtual void OnRep_PlayerState() override;
	// ���Ϳ� ���� �ʱ�ȭ �ϴ� �Լ�

	virtual int32 GetPlayerLevel() override;


private:
	virtual void InitAbilityActorInfo() override;

};
