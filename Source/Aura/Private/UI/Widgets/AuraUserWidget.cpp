// Copyright WhNi


#include "UI/Widgets/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWIdgetController)
{
	WidgetController = InWIdgetController;
	WidgetControllerSet();
}

