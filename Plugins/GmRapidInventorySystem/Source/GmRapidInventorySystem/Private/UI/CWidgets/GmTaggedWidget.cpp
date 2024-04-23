// Copyright 2024 Dev.GaeMyo. All Rights Reserved.


#include "UI/CWidgets/GmTaggedWidget.h"

UGmTaggedWidget::UGmTaggedWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGmTaggedWidget::SetVisibility(ESlateVisibility InVisibility)
{
#if WITH_EDITORONLY_DATA
	if (IsDesignTime())
	{
		Super::SetVisibility(InVisibility);
		return;
	}
#endif
	// Remember what the caller requested; even if we're currently being
	// suppressed by a tag we should respect this call when we're done
	bWantsToBeVisible = ConvertSerializedVisibilityToRuntime(InVisibility).IsVisible();
	if (bWantsToBeVisible)
	{
		ShownVisibility = InVisibility;
	}
	else
	{
		HiddenVisibility = InVisibility;
	}

	constexpr bool bHasHiddenTags{false};//@TODO: Foo->HasAnyTags(HiddenByTags);

	// Actually apply the visibility
	if (const ESlateVisibility DesiredVisibility{(bWantsToBeVisible && !bHasHiddenTags) ? ShownVisibility : HiddenVisibility}; GetVisibility() != DesiredVisibility)
	{
		Super::SetVisibility(DesiredVisibility);
	}
}

void UGmTaggedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsDesignTime())
	{
		// Listen for tag changes on our hidden tags
		//@TODO: That thing I said

		// Set our initial visibility value (checking the tags, etc...)
		SetVisibility(GetVisibility());
	}
}

void UGmTaggedWidget::NativeDestruct()
{
	if (!IsDesignTime())
	{
		//@TODO: Stop listening for tag changes
	}

	Super::NativeDestruct();
}

void UGmTaggedWidget::OnWatchedTagsChanged()
{
	constexpr bool bHasHiddenTags{false};//@TODO: Foo->HasAnyTags(HiddenByTags);

	// Actually apply the visibility
	if (const ESlateVisibility DesiredVisibility{(bWantsToBeVisible && !bHasHiddenTags) ? ShownVisibility : HiddenVisibility}; GetVisibility() != DesiredVisibility)
	{
		Super::SetVisibility(DesiredVisibility);
	}
}
