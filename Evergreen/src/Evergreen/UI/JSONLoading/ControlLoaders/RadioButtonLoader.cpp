#include "pch.h"
#include "RadioButtonLoader.h"

namespace Evergreen
{
bool RadioButtonLoader::ParseIsChecked(json& data)
{
	if (data.contains("IsChecked"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["IsChecked"].is_boolean(), "RadioButton control with name '{}': 'IsChecked' value must be a boolean. Invalid RadioButton object: {}", m_name, data.dump(4));
		return data["IsChecked"].get<bool>();
	}

	return false;
}

float RadioButtonLoader::ParseInnerRadius(json& data)
{
	if (data.contains("InnerRadius"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["InnerRadius"].is_number(), "RadioButton control with name '{}': 'InnerRadius' value must be a number. Invalid RadioButton object: {}", m_name, data.dump(4));
		float radius = data["InnerRadius"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radius > 0.0f, "RadioButton control with name '{}': 'InnerRadius' value must be greater than 0. Invalid RadioButton object: {}", m_name, data.dump(4));
		return radius;
	}

	return 3.0f;
}
float RadioButtonLoader::ParseOuterRadius(json& data)
{
	if (data.contains("OuterRadius"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OuterRadius"].is_number(), "RadioButton control with name '{}': 'OuterRadius' value must be a number. Invalid RadioButton object: {}", m_name, data.dump(4));
		float radius = data["OuterRadius"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radius > 0.0f, "RadioButton control with name '{}': 'OuterRadius' value must be greater than 0. Invalid RadioButton object: {}", m_name, data.dump(4));
		return radius;
	}

	return 6.0f;
}

std::unique_ptr<ColorBrush> RadioButtonLoader::ParseInnerBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	return data.contains("InnerBrush") ? std::move(JSONLoaders::LoadBrush(deviceResources, data["InnerBrush"])) : nullptr;
}
std::unique_ptr<ColorBrush> RadioButtonLoader::ParseOuterBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	return data.contains("OuterBrush") ? std::move(JSONLoaders::LoadBrush(deviceResources, data["OuterBrush"])) : nullptr;
}
void RadioButtonLoader::ParseOuterLineWidth(RadioButton* rb, json& data)
{
	EG_CORE_ASSERT(rb != nullptr, "radio button is nullptr");

	if (data.contains("OuterBrushLineWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OuterBrushLineWidth"].is_number(), "RadioButton control with name '{}': 'OuterBrushLineWidth' value must be a number. Invalid RadioButton object: {}", m_name, data.dump(4));
		float width = data["OuterBrushLineWidth"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(width > 0.0f, "RadioButton control with name '{}': 'OuterRadius' value must be greater than 0. Invalid RadioButton object: {}", m_name, data.dump(4));
		rb->SetOuterLineWidth(width);
	}
}
}