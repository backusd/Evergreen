#pragma once
#include "pch.h"
#include <Evergreen.h>

// Generic
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