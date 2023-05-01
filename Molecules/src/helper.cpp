#include "helper.h"

using namespace Evergreen;

const D2D1_COLOR_F g_menuBarButtonColorDefault = D2D1::ColorF(0.16f, 0.16f, 0.16f);
const D2D1_COLOR_F g_menuBarButtonColorMouseOverPaneClosed = D2D1::ColorF(0.35f, 0.35f, 0.35f);
const D2D1_COLOR_F g_menuBarButtonColorPaneOpen = D2D1::ColorF(0.25f, 0.25f, 0.25f);

void FileDropDownOnMouseEnter(Evergreen::Button* button, Evergreen::MouseMoveEvent& e)
{
	Pane* filePane = button->GetUI()->GetPane("FileDropDownPane");
	if (filePane->GetVisible())
	{
		//button->BackgroundBrush(g_menuBarButtonColorPaneOpen);
		button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
	}
	else
	{
		//button->BackgroundBrush(g_menuBarButtonColorMouseOverPaneClosed);
		button->BorderWidth(0.0f);
	}
}