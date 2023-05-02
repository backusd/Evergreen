#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "../../Simulation/Simulation.h"


// Menu Bar Buttons - Generic
void MenuBarButtonOnMouseEnter(Evergreen::Button* button, const std::string& paneName);
void MenuBarButtonOnMouseLButtonDown(Evergreen::Button* button, Evergreen::MouseButtonPressedEvent& e);
void MenuBarButtonOnClick(Evergreen::Button* button, const std::string& paneName);

// File Button
void FileDropDownOnMouseEnter(Evergreen::Button* button, Evergreen::MouseMoveEvent& e); 
void FileDropDownOnMouseLeave(Evergreen::Button* button, Evergreen::MouseMoveEvent& e); 
void FileDropDownOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);

// Edit Button
void EditDropDownOnMouseEnter(Evergreen::Button* button, Evergreen::MouseMoveEvent& e);
void EditDropDownOnMouseLeave(Evergreen::Button* button, Evergreen::MouseMoveEvent& e);
void EditDropDownOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);

// View Button
void ViewDropDownOnMouseEnter(Evergreen::Button* button, Evergreen::MouseMoveEvent& e);
void ViewDropDownOnMouseLeave(Evergreen::Button* button, Evergreen::MouseMoveEvent& e);
void ViewDropDownOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);

// -----------------------------------------------------------------------------------------------------
// Menu Bar Panes - Generic
void MenuBarDropDownPaneOnMouseExitedContentRegion(Evergreen::Pane* pane, Evergreen::MouseMoveEvent& e);
void MenuBarDropDownPaneButtonOnMouseEnter(Evergreen::Button* button, Evergreen::MouseMoveEvent& e);
void MenuBarDropDownPaneButtonOnMouseLeave(Evergreen::Button* button, Evergreen::MouseMoveEvent& e);
void MenuBarDropDownPaneButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e, const std::string& paneName, const std::string& buttonName);
void RightPanelAddTab(Evergreen::Button*& selectedTab, const std::string& tabButtonName, const std::string& tabButtonJSON, const std::string& contentJSON);

// FILE Pane
void FileDropDownNewButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);
void FileDropDownOpenButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);
void FileDropDownSaveButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);
void FileDropDownSaveAsButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);
void FileDropDownExitButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);

// EDIT
void EditDropDownCameraButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e, Evergreen::Button*& selectedTab);
void EditDropDownMaterialsButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e, Evergreen::Button*& selectedTab);
void EditDropDownLightingButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e, Evergreen::Button*& selectedTab);

// VIEW Pane
void ViewDropDownView1ButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);
void ViewDropDownView2ButtonOnClick(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e);