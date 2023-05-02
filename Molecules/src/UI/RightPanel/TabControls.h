#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "../../Simulation/Simulation.h"
#include "../../Rendering/Scene.h"

const D2D1_COLOR_F g_rightPanelTabColorSelected = D2D1::ColorF(0.16f, 0.16f, 0.16f);
const D2D1_COLOR_F g_rightPanelTabColorNotSelected = D2D1::ColorF(0.2f, 0.2f, 0.2f);
const D2D1_COLOR_F g_rightPanelTabColorMouseOver = D2D1::ColorF(0.25f, 0.25f, 0.25f);
const D2D1_COLOR_F g_rightPanelTabColorMouseDown = D2D1::ColorF(0.3f, 0.3f, 0.3f);

void MaterialEditElementSelectorDropDownItemOnClick(Evergreen::UI* ui, Scene* scene, const std::wstring& elementName, Element& currentElement, Element newElement);

// Right Panel Tabs - Generic -------------------------------------------------------------------
void RightPanelTabChangeBackgroundAndCloseButtonColor(Evergreen::Button* button, const D2D1_COLOR_F& background, D2D1::ColorF::Enum text);

// Tab Button
void RightPanelTabOnMouseEnter(Evergreen::Button* button, Evergreen::MouseMoveEvent& e, Evergreen::Button* selectedTab);
void RightPanelTabOnMouseLeave(Evergreen::Button* button, Evergreen::MouseMoveEvent& e, Evergreen::Button* selectedTab);
void RightPanelTabOnMouseLButtonDown(Evergreen::Button* button, Evergreen::MouseButtonPressedEvent& e, Evergreen::Button* selectedTab);

// Close Button
void RightPanelTabCloseButtonOnMouseEnter(Evergreen::Button* button, Evergreen::MouseMoveEvent& e);
void RightPanelTabCloseButtonOnMouseLeave(Evergreen::Button* button, Evergreen::MouseMoveEvent& e);
void RightPanelTabCloseButtonOnMouseLButtonDown(Evergreen::Button* button, Evergreen::MouseButtonPressedEvent& e);
