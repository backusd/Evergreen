#include "TabControls.h"

using namespace Evergreen;

void MaterialEditElementSelectorDropDownItemOnClick(UI* ui, Scene* scene, const std::wstring& elementName, Element& currentElement, Element newElement)
{
	EG_ASSERT(ui != nullptr, "UI cannot be nullptr");
	EG_ASSERT(scene != nullptr, "Scene cannot be nullptr");

	Text* dropDownText = ui->GetControlByName<Text>("RightPanel_ElementSelectorDropDown_Text");
	EG_ASSERT(dropDownText != nullptr, "Could not find drop down text");
	dropDownText->SetText(elementName);

	currentElement = newElement;

	Pane* pane = ui->GetPane("RightPanel_ElementSelectorDropDown_Pane");
	EG_ASSERT(pane != nullptr, "Pane not found");
	pane->SetVisible(false);

	// Update the sliders that can edit the material
	MaterialsArray* materials = scene->GetMaterials();
	EG_ASSERT(materials != nullptr, "materials not found");

	const Material& material = materials->materials[static_cast<int>(currentElement) - 1];

	Layout* rightPanelContentLayout = ui->GetLayoutByName("RightPanelLayout_ContentLayout");
	EG_ASSERT(rightPanelContentLayout != nullptr, "Could not find layout");

	SliderFloat* diffuseAlbedoX = rightPanelContentLayout->GetControlByName<SliderFloat>("MaterialDiffuseAlbedoXSlider");
	EG_ASSERT(diffuseAlbedoX != nullptr, "diffuseAlbedoX not found");
	diffuseAlbedoX->SetValue(material.DiffuseAlbedo.x);

	SliderFloat* diffuseAlbedoY = rightPanelContentLayout->GetControlByName<SliderFloat>("MaterialDiffuseAlbedoYSlider");
	EG_ASSERT(diffuseAlbedoY != nullptr, "diffuseAlbedoY not found");
	diffuseAlbedoY->SetValue(material.DiffuseAlbedo.y);

	SliderFloat* diffuseAlbedoZ = rightPanelContentLayout->GetControlByName<SliderFloat>("MaterialDiffuseAlbedoZSlider");
	EG_ASSERT(diffuseAlbedoZ != nullptr, "diffuseAlbedoZ not found");
	diffuseAlbedoZ->SetValue(material.DiffuseAlbedo.z);

	SliderFloat* fresnelX = rightPanelContentLayout->GetControlByName<SliderFloat>("MaterialFresnelXSlider");
	EG_ASSERT(fresnelX != nullptr, "fresnelX not found");
	fresnelX->SetValue(material.FresnelR0.x);

	SliderFloat* fresnelY = rightPanelContentLayout->GetControlByName<SliderFloat>("MaterialFresnelYSlider");
	EG_ASSERT(fresnelY != nullptr, "fresnelY not found");
	fresnelY->SetValue(material.FresnelR0.y);

	SliderFloat* fresnelZ = rightPanelContentLayout->GetControlByName<SliderFloat>("MaterialFresnelZSlider");
	EG_ASSERT(fresnelZ != nullptr, "fresnelZ not found");
	fresnelZ->SetValue(material.FresnelR0.z);

	SliderFloat* shininess = rightPanelContentLayout->GetControlByName<SliderFloat>("MaterialShininessSlider");
	EG_ASSERT(shininess != nullptr, "shininess not found");
	shininess->SetValue(material.Shininess);
}

// Right Panel Tabs - Generic -------------------------------------------------------------------
void RightPanelTabChangeBackgroundAndCloseButtonColor(Evergreen::Button* button, const D2D1_COLOR_F& background, D2D1::ColorF::Enum text)
{
	button->BackgroundBrushAndTextColor(background, text);
	Button* closeButton = static_cast<Button*>(button->GetLayout()->GetFirstControlOfType(Control::ControlType::Button)); 
	if (closeButton != nullptr) 
	{
		closeButton->BackgroundBrushAndTextColor(background, text);
	}
}

// Tab Button
void RightPanelTabOnMouseEnter(Button* button, MouseMoveEvent& e, Button* selectedTab)
{
	if (button != selectedTab)
		RightPanelTabChangeBackgroundAndCloseButtonColor(button, g_rightPanelTabColorMouseOver, D2D1::ColorF::White);
}
void RightPanelTabOnMouseLeave(Button* button, MouseMoveEvent& e, Button* selectedTab)
{
	if (button != selectedTab)
		RightPanelTabChangeBackgroundAndCloseButtonColor(button, g_rightPanelTabColorNotSelected, D2D1::ColorF::LightGray);
}
void RightPanelTabOnMouseLButtonDown(Button* button, MouseButtonPressedEvent& e, Button* selectedTab)
{
	if (button != selectedTab)
		RightPanelTabChangeBackgroundAndCloseButtonColor(button, g_rightPanelTabColorMouseDown, D2D1::ColorF::White);
}

// Close Button
void RightPanelTabCloseButtonOnMouseEnter(Button* button, MouseMoveEvent& e)
{
	button->BackgroundBrushAndTextColor(D2D1::ColorF::White, D2D1::ColorF::Black);
}
void RightPanelTabCloseButtonOnMouseLeave(Button* button, MouseMoveEvent& e)
{
	button->BackgroundBrushAndTextColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f), D2D1::ColorF::White);
}
void RightPanelTabCloseButtonOnMouseLButtonDown(Button* button, MouseButtonPressedEvent& e)
{
	button->BackgroundBrushAndTextColor(D2D1::ColorF::White, D2D1::ColorF::Gray);

}