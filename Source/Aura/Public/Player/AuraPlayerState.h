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


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ���������� ������Ʈ�� ���� ���� getter�� ������ �־�� ��.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UAttributeSet* GetAttributeSet()  const { return AttributeSet; }

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }

protected:

	// �� Ŭ������ ���ؼ� ĳ������ ������ ������ �������.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

};
