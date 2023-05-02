#include "MenuBarControls.h"
#include "../../Rendering/Scene.h"

using namespace Evergreen;

const D2D1_COLOR_F g_menuBarButtonColorDefault = D2D1::ColorF(0.16f, 0.16f, 0.16f);
const D2D1_COLOR_F g_menuBarButtonColorMouseOverPaneClosed = D2D1::ColorF(0.35f, 0.35f, 0.35f);
const D2D1_COLOR_F g_menuBarButtonColorPaneOpen = D2D1::ColorF(0.25f, 0.25f, 0.25f);

const D2D1_COLOR_F g_rightPanelTabColorSelected = D2D1::ColorF(0.16f, 0.16f, 0.16f);
const D2D1_COLOR_F g_rightPanelTabColorNotSelected = D2D1::ColorF(0.2f, 0.2f, 0.2f);
const D2D1_COLOR_F g_rightPanelTabColorMouseOver = D2D1::ColorF(0.25f, 0.25f, 0.25f);
const D2D1_COLOR_F g_rightPanelTabColorMouseDown = D2D1::ColorF(0.3f, 0.3f, 0.3f);


// -----------------------------------------------------------------------------------------------------
// Menu Bar Buttons - Generic
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

// File Button
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

// Edit Button
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

// View Button
void ViewDropDownOnMouseEnter(Button* button, MouseMoveEvent& e)
{
	MenuBarButtonOnMouseEnter(button, "ViewDropDownPane");
}
void ViewDropDownOnMouseLeave(Button* button, MouseMoveEvent& e)
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
void ViewDropDownOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarButtonOnClick(button, "ViewDropDownPane");
}


// -----------------------------------------------------------------------------------------------------
// Menu Bar Panes - Generic
void MenuBarDropDownPaneOnMouseExitedContentRegion(Pane* pane, MouseMoveEvent& e)
{
	UI* ui = pane->GetUI();

	Pane* filePane = ui->GetPane("FileDropDownPane");
	Pane* editPane = ui->GetPane("EditDropDownPane");
	Pane* viewPane = ui->GetPane("ViewDropDownPane");
	Button* fileButton = ui->GetControlByName<Button>("FileDropDownButton");
	Button* editButton = ui->GetControlByName<Button>("EditDropDownButton");
	Button* viewButton = ui->GetControlByName<Button>("ViewDropDownButton");

	if (fileButton->ContainsPoint(e.GetX(), e.GetY()))
	{
		if (filePane != pane)
		{
			pane->SetVisible(false);
			filePane->SetVisible(true);

			if (pane == editPane)
			{
				editButton->BackgroundBrush(g_menuBarButtonColorDefault);
				editButton->BorderWidth(0.0f);
			}
			else if (pane == viewPane)
			{
				viewButton->BackgroundBrush(g_menuBarButtonColorDefault);
				viewButton->BorderWidth(0.0f);
			}
		}
	}
	else if (editButton->ContainsPoint(e.GetX(), e.GetY()))
	{
		if (editPane != pane)
		{
			pane->SetVisible(false);
			editPane->SetVisible(true);

			if (pane == filePane)
			{
				fileButton->BackgroundBrush(g_menuBarButtonColorDefault);
				fileButton->BorderWidth(0.0f);
			}
			else if (pane == viewPane)
			{
				viewButton->BackgroundBrush(g_menuBarButtonColorDefault);
				viewButton->BorderWidth(0.0f);
			}
		}
	}
	else if (viewButton->ContainsPoint(e.GetX(), e.GetY()))
	{
		if (viewPane != pane)
		{
			pane->SetVisible(false);
			viewPane->SetVisible(true);

			if (pane == filePane)
			{
				fileButton->BackgroundBrush(g_menuBarButtonColorDefault);
				fileButton->BorderWidth(0.0f);
			}
			else if (pane == editPane)
			{
				editButton->BackgroundBrush(g_menuBarButtonColorDefault);
				editButton->BorderWidth(0.0f);
			}
		}
	}
	else
	{
		// Mouse is not over pane or menu bar button, so collapse it
		pane->SetVisible(false);
		if (pane == filePane)
		{
			fileButton->BackgroundBrush(g_menuBarButtonColorDefault);
			fileButton->BorderWidth(0.0f);
		}
		else if (pane == editPane)
		{
			editButton->BackgroundBrush(g_menuBarButtonColorDefault);
			editButton->BorderWidth(0.0f);
		}
		else if (pane == viewPane)
		{
			viewButton->BackgroundBrush(g_menuBarButtonColorDefault);
			viewButton->BorderWidth(0.0f);
		}
	}
}
void MenuBarDropDownPaneButtonOnMouseEnter(Button* button, MouseMoveEvent&)
{
	button->BackgroundBrush(g_menuBarButtonColorMouseOverPaneClosed);
}
void MenuBarDropDownPaneButtonOnMouseLeave(Button* button, MouseMoveEvent&)
{
	button->BackgroundBrush(g_menuBarButtonColorPaneOpen);
}
void MenuBarDropDownPaneButtonOnClick(Button* button, MouseButtonReleasedEvent& e, const std::string& paneName, const std::string& buttonName)
{
	UI* ui = button->GetUI();

	// Reset the color of the button
	button->BackgroundBrush(g_menuBarButtonColorPaneOpen);

	// Close the pane
	Pane* filePane = ui->GetPane(paneName);
	filePane->SetVisible(false);

	// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
	// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
	// To do this, we inform the event that it should ignore the handling control
	e.IgnoreHandlingControl(true);

	Button* menuBarButton = ui->GetControlByName<Button>(buttonName);
	menuBarButton->BackgroundBrush(g_menuBarButtonColorDefault);
	menuBarButton->BorderWidth(0.0f);
}
void RightPanelAddTab(Button*& selectedTab, const std::string& tabButtonName, const std::string& tabButtonJSON, const std::string& contentJSON)
{
	EG_ASSERT(selectedTab != nullptr, "selected tab cannot but nullptr");

	UI* ui = selectedTab->GetUI();
	EG_ASSERT(ui != nullptr, "UI cannot but nullptr");

	// Get the layout for the right panel tabs area
	Layout* rightPanelTabsLayout = ui->GetLayoutByName("RightPanelLayout_TopButtonsLayout");
	EG_ASSERT(rightPanelTabsLayout != nullptr, "layout not found");

	Layout* rightPanelContentLayout = ui->GetLayoutByName("RightPanelLayout_ContentLayout");
	EG_ASSERT(rightPanelContentLayout != nullptr, "layout not found");

	// Determine if the tab already exists. If not, create it			
	Button* tabButton = rightPanelTabsLayout->GetControlByName<Button>(tabButtonName);
	if (tabButton == nullptr)
	{
		// We are going to force the "Simulation" tab to not be removable. When we add a second tab, it will
		// be placed in the second column, which already exists. However, when we add a third, fourth, etc tab,
		// we have to add an additional column to the layout
		if (rightPanelTabsLayout->NumberOfControls() > 1)
			rightPanelTabsLayout->AddColumn({ RowColumnType::STAR, 1.0f });

		RowColumnPosition rowCol = { 0, static_cast<unsigned int>(rightPanelTabsLayout->Columns().size()) - 1, 1, 1 };
		ui->LoadControlsFromFile(tabButtonJSON, rightPanelTabsLayout, rowCol);

		tabButton = rightPanelTabsLayout->GetControlByName<Button>(tabButtonName);
		EG_ASSERT(tabButton != nullptr, std::format("Failed to create/find {} button", tabButtonName));
	}

	// If the tabButton is already selected, there is nothing else we need to do
	if (selectedTab == tabButton)
		return;

	// BEFORE updating the rightPaneSelectedTabButton pointer, update the current Button
	selectedTab->BackgroundBrushAndTextColor(g_rightPanelTabColorNotSelected, D2D1::ColorF::LightGray);
	Button* closeButton = static_cast<Button*>(selectedTab->GetLayout()->GetFirstControlOfType(Control::ControlType::Button));
	if (closeButton != nullptr)
		closeButton->BackgroundBrushAndTextColor(g_rightPanelTabColorNotSelected, D2D1::ColorF::LightGray);

	// Keep track of the selected tab
	selectedTab = tabButton;

	// Update the content layouts border along the top edge
	const D2D1_RECT_F& buttonRect = tabButton->BackgroundRect();
	rightPanelContentLayout->BorderTopLeftOffsetX(buttonRect.left - rightPanelContentLayout->Left());
	rightPanelContentLayout->BorderTopRightOffsetX(rightPanelContentLayout->Right() - buttonRect.right);

	// Load the material editing controls in the right pane
	ui->LoadLayoutFromFile(contentJSON, rightPanelContentLayout); 
}

// FILE Pane
void FileDropDownNewButtonOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "FileDropDownPane", "FileDropDownButton");
}
void FileDropDownOpenButtonOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "FileDropDownPane", "FileDropDownButton");
}
void FileDropDownSaveButtonOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "FileDropDownPane", "FileDropDownButton");
}
void FileDropDownSaveAsButtonOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "FileDropDownPane", "FileDropDownButton");
}
void FileDropDownExitButtonOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "FileDropDownPane", "FileDropDownButton");
}

// EDIT
void EditDropDownCameraButtonOnClick(Button* button, MouseButtonReleasedEvent& e, Button*& selectedTab)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "EditDropDownPane", "EditDropDownButton");
	RightPanelAddTab(selectedTab, "RightPanel_CameraButton", "right_panel_camera_tab.json", "right_panel_camera_content.json");
}
void EditDropDownMaterialsButtonOnClick(Button* button, MouseButtonReleasedEvent& e, Button*& selectedTab)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "EditDropDownPane", "EditDropDownButton");
	RightPanelAddTab(selectedTab, "RightPanel_MaterialsButton", "right_panel_materials_tab.json", "right_panel_materials_content.json");
}
void EditDropDownLightingButtonOnClick(Button* button, MouseButtonReleasedEvent& e, Button*& selectedTab)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "EditDropDownPane", "EditDropDownButton");
	RightPanelAddTab(selectedTab, "RightPanel_LightingButton", "right_panel_lighting_tab.json", "right_panel_lighting_content.json");
}

// VIEW Pane
void ViewDropDownView1ButtonOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "ViewDropDownPane", "ViewDropDownButton");
}
void ViewDropDownView2ButtonOnClick(Button* button, MouseButtonReleasedEvent& e)
{
	MenuBarDropDownPaneButtonOnClick(button, e, "ViewDropDownPane", "ViewDropDownButton");
}