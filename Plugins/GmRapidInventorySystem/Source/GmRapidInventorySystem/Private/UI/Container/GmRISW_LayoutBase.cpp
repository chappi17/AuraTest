// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#include "UI/Container/GmRISW_LayoutBase.h"

#include <Editor/WidgetCompilerLog.h>

// UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");

#define LOCTEXT_NAMESPACE "GmRIS"

UGmRISW_LayoutBase::UGmRISW_LayoutBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

#if WITH_EDITOR
void UGmRISW_LayoutBase::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,
	IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);

	if (!GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UGmRISW_LayoutBase, BP_GetDesiredFocusTarget)))
	{
		if (GetParentNativeClass(GetClass()) == /*UGmRISW_LayoutBase::*/StaticClass())
		{
			CompileLog.Warning(LOCTEXT("ValidateGetDesiredFocusTarget_Warning",
				"Looks like someone skipped implementing GetDesiredFocusTarget. If you're using a gamepad, prepare for an unexpected adventure on this screen! - By Dev.GaeMyo"));
		}
		else
		{
			//TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
			CompileLog.Note(LOCTEXT("ValidateGetDesiredFocusTarget_Note",
				"GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.  If it was implemented in the native base class you can ignore this message. - By Dev.GaeMyo"));
		}
	}
}
#endif

// void UGmRISW_LayoutBase::HandleEscapeAction()
// {
	// if (ensure(!EscapeMenuClass.IsNull()))
	// {
		// UGmAdvancedUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_MENU, EscapeMenuClass);
	// }
// }

#undef LOCTEXT_NAMESPACE
