#include "MenuBarButtons.h"

using namespace Evergreen;

const D2D1_COLOR_F g_menuBarButtonColorDefault = D2D1::ColorF(0.16f, 0.16f, 0.16f);
const D2D1_COLOR_F g_menuBarButtonColorMouseOverPaneClosed = D2D1::ColorF(0.35f, 0.35f, 0.35f);
const D2D1_COLOR_F g_menuBarButtonColorPaneOpen = D2D1::ColorF(0.25f, 0.25f, 0.25f);

void FileDropDownButton::HandleOnMouseEntered(MouseMoveEvent&)
{
	if (m_ui->PaneExists("FileDropDownPane"))
	{
		BackgroundBrush(g_menuBarButtonColorPaneOpen);
		BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
	}
	else
	{
		BackgroundBrush(g_menuBarButtonColorMouseOverPaneClosed);
		BorderWidth(0.0f);
	}
}
void FileDropDownButton::HandleOnMouseExited(MouseMoveEvent&)
{
	//Pane* filePane = m_ui->GetPane("FileDropDownPane");

	// First if the file pane is visible, check if the mouse is now over the Edit button
	if (m_ui->PaneExists("FileDropDownPane"))
	{
		EditDropDownButton* editButton = m_ui->GetControlByName<EditDropDownButton>("EditDropDownButton");
	//	if (editButton->ContainsPoint(e.GetX(), e.GetY()))
	//	{
	//		filePane->SetVisible(false);
	//		this->ChangeButtonBackground(button, m_menuBarButtonColorDefault);
	//		button->BorderWidth(0.0f);
	//
	//		Pane* editPane = m_ui->GetPane("EditDropDownPane");
	//		editPane->SetVisible(true);
	//	}
	//	else if (filePane->ContainsPoint(e.GetX(), e.GetY()))
	//	{
	//		this->ChangeButtonBackground(button, m_menuBarButtonColorPaneOpen);
	//	}
	//	else
	//	{
	//		// Mouse is not over the button or pane, so close the pane
	//		filePane->SetVisible(false);
	//		this->ChangeButtonBackground(button, m_menuBarButtonColorDefault);
	//		button->BorderWidth(0.0f);
	//	}
	}
	else
	{
		BackgroundBrush(g_menuBarButtonColorDefault);
		BorderWidth(0.0f);
	}
}
void FileDropDownButton::HandleOnMouseMove(MouseMoveEvent&) {}
void FileDropDownButton::HandleOnMouseLButtonDown(MouseButtonPressedEvent&)
{

}
void FileDropDownButton::HandleOnClick(MouseButtonReleasedEvent&)
{

}

// ==================================================================

void EditDropDownButton::HandleOnMouseEntered(MouseMoveEvent&)
{

}
void EditDropDownButton::HandleOnMouseExited(MouseMoveEvent&)
{

}
void EditDropDownButton::HandleOnMouseMove(MouseMoveEvent&)
{

}
void EditDropDownButton::HandleOnMouseLButtonDown(MouseButtonPressedEvent&)
{

}
void EditDropDownButton::HandleOnClick(MouseButtonReleasedEvent&)
{

}

// ==================================================================

void ViewDropDownButton::HandleOnMouseEntered(MouseMoveEvent&)
{

}
void ViewDropDownButton::HandleOnMouseExited(MouseMoveEvent&)
{

}
void ViewDropDownButton::HandleOnMouseMove(MouseMoveEvent&)
{

}
void ViewDropDownButton::HandleOnMouseLButtonDown(MouseButtonPressedEvent&)
{

}
void ViewDropDownButton::HandleOnClick(MouseButtonReleasedEvent&)
{

}

// ==================================================================