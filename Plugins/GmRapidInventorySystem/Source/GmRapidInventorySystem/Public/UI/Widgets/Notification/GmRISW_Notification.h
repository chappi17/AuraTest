// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "GmRISW_Notification.generated.h"

class UCommonLazyImage;
class UTextBlock;

UCLASS()
class GMRAPIDINVENTORYSYSTEM_API UGmRISW_Notification : public UUserWidget
{
	GENERATED_BODY()

public:

	float FadeOutDelay{2.f};
	FVector2D Forward_Reverse_WidgetAnimPlayRate{FVector2D{1.f, 1.f}};
	FText NotificationTitleText{FText::FromString(L"Picked")};
	FText NotificationContentText{FText::FromString(L"Notify Content")};
	int32 NotificationsStackCount{999};
	FSlateBrush NotificationIconBrush;

protected:

	// Properties
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationTitle;

	// Properties
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationsStack;

	// Properties
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationContent;

	// Properties
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> NotificationIcon;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> AppearanceAnim;

	//~UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUserWidget Interface


private:
	
};
