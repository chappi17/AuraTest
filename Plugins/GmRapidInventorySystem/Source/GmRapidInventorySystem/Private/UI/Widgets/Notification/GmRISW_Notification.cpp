// Copyright Dev.GaeMyo 2024. All Rights Reserved.


#include "UI/Widgets/Notification/GmRISW_Notification.h"

#include <string>

#include <CommonLazyImage.h>
#include <TimerManager.h>
#include <Animation/WidgetAnimation.h>
#include <Engine/World.h>
#include <Components/TextBlock.h>

void UGmRISW_Notification::NativeConstruct()
{
	Super::NativeConstruct();

	NotificationTitle->SetText(NotificationTitleText);
	NotificationContent->SetText(NotificationContentText);
	NotificationIcon->SetBrush(NotificationIconBrush);
	NotificationsStack->SetText(FText::FromString(FString(L"x") + std::to_string(NotificationsStackCount).c_str()));

	if (const UWorld* CurrentWorld{GetWorld()})
	{
		PlayAnimationForward(AppearanceAnim, Forward_Reverse_WidgetAnimPlayRate.X);

		FTimerHandle Th;
		CurrentWorld->GetTimerManager().SetTimer(Th, FTimerDelegate::CreateLambda([this]()->void
		{
			if (const UWorld* NewCurrentWorld{GetWorld()})
			{
				FTimerHandle Th2;
				NewCurrentWorld->GetTimerManager().SetTimer(Th2, FTimerDelegate::CreateLambda([this, NewCurrentWorld]()->void
				{
					PlayAnimationReverse(AppearanceAnim, Forward_Reverse_WidgetAnimPlayRate.Y);

					FTimerHandle Th3;
					NewCurrentWorld->GetTimerManager().SetTimer(Th3, FTimerDelegate::CreateLambda([this]()->void
					{
						RemoveFromParent();
					}), AppearanceAnim->GetEndTime() / Forward_Reverse_WidgetAnimPlayRate.Y, false);
				}), FadeOutDelay, false);
			}
		}), AppearanceAnim->GetEndTime() / Forward_Reverse_WidgetAnimPlayRate.X, false);
	}
}

void UGmRISW_Notification::NativeDestruct()
{
	Super::NativeDestruct();
}
