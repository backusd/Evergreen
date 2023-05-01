#include "MenuBarControls.h"

using namespace Evergreen;

const D2D1_COLOR_F g_menuBarButtonColorDefault = D2D1::ColorF(0.16f, 0.16f, 0.16f);
const D2D1_COLOR_F g_menuBarButtonColorMouseOverPaneClosed = D2D1::ColorF(0.35f, 0.35f, 0.35f);
const D2D1_COLOR_F g_menuBarButtonColorPaneOpen = D2D1::ColorF(0.25f, 0.25f, 0.25f);

// Generic --------------------------------------------------------------------------------
void MenuBarButtonOnMouseEnter(Button* button, const std::string& paneName)
{
	Pane* filePane = button->GetUI()->GetPane(paneName);
	if (filePane->GetVisible())
	{
		button->BackgroundBrush(g_menuBarButtonColorPaneOpen);
		button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
	}
	else
	{
		button->BackgroundBrush(g_menuBarButtonColorMouseOverPaneClosed);
		button->BorderWidth(0.0f);
	}
}
void MenuBarButtonOnMouseLButtonDown(Button* button, MouseButtonPressedEvent&)
{
	button->BackgroundBrush(g_menuBarButtonColorPaneOpen);
}
void MenuBarButtonOnClick(Button* button, const std::string& paneName)
{
	Pane* filePane = button->GetUI()->GetPane(paneName);
	filePane->SwitchVisible();

	if (filePane->GetVisible())
	{
		button->BackgroundBrush(g_menuBarButtonColorPaneOpen);
		button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
	}
	else if (button->MouseIsOver())
	{
		button->BackgroundBrush(g_menuBarButtonColorMouseOverPaneClosed);
		button->BorderWidth(0.0f);
	}
}

// File Button ----------------------------------------------------------------------------
void FileDropDownOnMouseEnter(Button* button, MouseMoveEvent& e)
{
	MenuBarButtonOnMouseEnter(button, "FileDropDownPane");
}
void FileDropDownOnMouseLeave(Button* button, MouseMoveEvent& e)
{
	UI* ui = button->GetUI();
	Pane* filePane = ui->GetPane("FileDropDownPane");

	// First if the file pane is visible, check if the mouse is now over the Edit button
	if (filePane->GetVisible())
	{
		Button* editButton = ui->GetControlByName<Button>("EditDropDownButton");
		if (editButton->ContainsPoint(e.GetX(), e.GetY()))
		{
			filePane->SetVisible(false);
			button->BackgroundBrush(g_menuBarButtonColorDefault);
			button->BorderWidth(0.0f);

			Pane* editPane = ui->GetPane("EditDropDownPane");
			editPane->SetVisible(true);
		}
		else if (filePane->ContainsPoint(e.GetX(), e.GetY()))
		{
			button->BackgroundBrush(g_menuBarButtonColorPaneOpen);
		}
		else
		{
			// Mouse is not over the button or pane, so close the pane
			filePane->SetVisible(false);
			button->BackgroundBrush(g_menuBarButtonColorDefault);
			button->BorderWidth(0.0f);
		}
	}
	else
	{
		button->BackgroundBrush(g_menuBarButtonColorDefault);
	}
}
void FileDropDownOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarButtonOnClick(button, "FileDropDownPane");
}

// Edit Button ----------------------------------------------------------------------------
void EditDropDownOnMouseEnter(Button* button, MouseMoveEvent& e)
{
	MenuBarButtonOnMouseEnter(button, "EditDropDownPane");
}
void EditDropDownOnMouseLeave(Button* button, MouseMoveEvent& e)
{
	UI* ui = button->GetUI(); 
	Pane* editPane = ui->GetPane("EditDropDownPane");

	// First if the edit pane is visible, check if the mouse is now over the File button or View Button
	if (editPane->GetVisible())
	{
		Button* fileButton = ui->GetControlByName<Button>("FileDropDownButton");
		Button* viewButton = ui->GetControlByName<Button>("ViewDropDownButton");
		MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e); 
		if (fileButton->ContainsPoint(mme.GetX(), mme.GetY()))
		{
			editPane->SetVisible(false);
			button->BackgroundBrush(g_menuBarButtonColorDefault);
			button->BorderWidth(0.0f);

			Pane* filePane = ui->GetPane("FileDropDownPane");
			filePane->SetVisible(true);
		}
		else if (viewButton->ContainsPoint(mme.GetX(), mme.GetY()))
		{
			editPane->SetVisible(false);
			button->BackgroundBrush(g_menuBarButtonColorDefault);
			button->BorderWidth(0.0f);

			Pane* viewPane = ui->GetPane("ViewDropDownPane");
			viewPane->SetVisible(true);
		}
		else if (editPane->ContainsPoint(mme.GetX(), mme.GetY()))
		{
			button->BackgroundBrush(g_menuBarButtonColorPaneOpen);
		}
		else
		{
			// Mouse is not over the button or pane, so close the pane
			editPane->SetVisible(false);
			button->BackgroundBrush(g_menuBarButtonColorDefault);
			button->BorderWidth(0.0f);
		}
	}
	else
	{
		button->BackgroundBrush(g_menuBarButtonColorDefault);
	}
}
void EditDropDownOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarButtonOnClick(button, "EditDropDownPane");
}

// View Button ----------------------------------------------------------------------------
void ViewDropDownOnMouseEnter(Evergreen::Button* button, Evergreen::MouseMoveEvent& e)
{
	MenuBarButtonOnMouseEnter(button, "ViewDropDownPane");
}
void ViewDropDownOnMouseLeave(Evergreen::Button* button, Evergreen::MouseMoveEvent& e)
{
	UI* ui = button->GetUI();
	Pane* viewPane = ui->GetPane("ViewDropDownPane");

	// First if the view pane is visible, check if the mouse is now over the Edit button
	if (viewPane->GetVisible())
	{
		Button* editButton = ui->GetControlByName<Button>("EditDropDownButton");
		MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);
		if (editButton->ContainsPoint(mme.GetX(), mme.GetY()))
		{
			viewPane->SetVisible(false);
			button->BackgroundBrush(g_menuBarButtonColorDefault);
			button->BorderWidth(0.0f);

			Pane* editPane = ui->GetPane("EditDropDownPane");
			editPane->SetVisible(true);
		}
		else if (viewPane->ContainsPoint(mme.GetX(), mme.GetY()))
		{
			button->BackgroundBrush(g_menuBarButtonColorPaneOpen);
		}
		else
		{
			// Mouse is not over the button or pane, so close the pane
			viewPane->SetVisible(false);
			button->BackgroundBrush(g_menuBarButtonColorDefault);
			button->BorderWidth(0.0f);
		}
	}
	else
	{
		button->BackgroundBrush(g_menuBarButtonColorDefault);
	}
}
void ViewDropDownOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e)
{
	MenuBarButtonOnClick(button, "ViewDropDownPane");
}