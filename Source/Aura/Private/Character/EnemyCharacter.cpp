// Copyright WhNi
#include "Character/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Aura/Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AEnemyCharacter::AEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// GAS ������Ʈ �����ϰ� ���ø����̼� ���� �������
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	// AI�� �ַ� Minimal mode�� �����ؾ���. 
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Attribute ���� ��������
	AttributeSet = CreateDefaultSubobject<UAttributeSet>("ArrtibuteSet");
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
}

void AEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}

void AEnemyCharacter::HighlightActor()
{
	// �޽� ���̶���Ʈ
	GetMesh()->SetRenderCustomDepth(true);
	// CustomDepthStencil 
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	// ���⵵ ���̶���Ʈ ����ruf
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AEnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AEnemyCharacter::GetPlayerLevel()
{

	return Level;
}

