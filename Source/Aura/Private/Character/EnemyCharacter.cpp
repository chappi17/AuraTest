// Copyright WhNi
#include "GameFramework/CharacterMovementComponent.h"
#include "Aura/Aura.h"
#include "Character/EnemyCharacter.h"

AEnemyCharacter::AEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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
