#include "pch.h"
#include "RadioButtonLoader.h"

namespace Evergreen
{
Control* RadioButtonLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique

	RowColumnPosition rowCol;
	Margin margin;

	// Parse Row/Column
	rowCol = ParseRowColumnPosition(data);

	// Parse Margin
	margin = ParseMargin(data);

	// Parse IsChecked
	bool isChecked = ParseIsChecked(data);

	// Parse inner & outer radius
	float innerRadius = ParseInnerRadius(data);
	float outerRadius = ParseOuterRadius(data);
	JSON_LOADER_EXCEPTION_IF_FALSE(innerRadius < outerRadius, "RadioButton control with name '{}': 'InnerRadius' value must be less than 'OuterRadius' value. Invalid RadioButton object: {}", m_name, data.dump(4));

	// Parse Brushes
	std::unique_ptr<ColorBrush> innerBrush = std::move(ParseInnerBrush(deviceResources, data));
	std::unique_ptr<ColorBrush> outerBrush = std::move(ParseOuterBrush(deviceResources, data));

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"IsChecked", "InnerRadius", "OuterRadius", "InnerBrush", "OuterBrush", "OuterBrushLineWidth",
	"OnMouseEntered", "OnMouseExited", "OnMouseMoved", "OnMouseLButtonDown", "OnMouseLButtonUp", "OnIsCheckedChanged" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Button control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	RadioButton* rb = parent->CreateControl<RadioButton>(
		rowCol, 
		deviceResources, 
		isChecked,
		outerRadius,
		innerRadius,
		std::move(outerBrush),
		std::move(innerBrush),
		margin
	);
	EG_CORE_ASSERT(rb != nullptr, "Something went wrong, radiobutton is nullptr");

	rb->Name(name);
	rb->ID(ParseID(data));

	ParseOuterLineWidth(rb, data);

	ParseOnMouseEntered(rb, data);
	ParseOnMouseExited(rb, data);
	ParseOnMouseMoved(rb, data);
	ParseOnMouseLButtonDown(rb, data);
	ParseOnMouseLButtonUp(rb, data);
	ParseOnIsCheckedChanged(rb, data);

	return rb;
}

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
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("InnerBrush"), "RadioButton control with name '{}': No 'InnerBrush' key. Incomplete RadioButton object: {}", m_name, data.dump(4));
	return std::move(JSONLoaders::LoadBrush(deviceResources, data["InnerBrush"]));
}
std::unique_ptr<ColorBrush> RadioButtonLoader::ParseOuterBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("OuterBrush"), "RadioButton control with name '{}': No 'OuterBrush' key. Incomplete RadioButton object: {}", m_name, data.dump(4));
	return std::move(JSONLoaders::LoadBrush(deviceResources, data["OuterBrush"]));
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

void RadioButtonLoader::ParseOnMouseEntered(RadioButton* rb, json& data)
{
	EG_CORE_ASSERT(rb != nullptr, "radio button is nullptr");

	if (data.contains("OnMouseEntered"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseEntered"].is_string(), "RadioButton control with name '{}': 'OnMouseEntered' value must be a string. Invalid RadioButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseEntered"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<RadioButton, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RadioButton control with name '{}': 'OnMouseEntered' callback not found for key '{}'. Invalid RadioButton object: {}", m_name, key, data.dump(4));
		rb->SetOnMouseEnteredCallback(callback);
	}
}
void RadioButtonLoader::ParseOnMouseExited(RadioButton* rb, json& data)
{
	EG_CORE_ASSERT(rb != nullptr, "radio button is nullptr");

	if (data.contains("OnMouseExited"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseExited"].is_string(), "RadioButton control with name '{}': 'OnMouseExited' value must be a string. Invalid RadioButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseExited"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<RadioButton, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RadioButton control with name '{}': 'OnMouseExited' callback not found for key '{}'. Invalid RadioButton object: {}", m_name, key, data.dump(4));
		rb->SetOnMouseExitedCallback(callback);
	}
}
void RadioButtonLoader::ParseOnMouseMoved(RadioButton* rb, json& data)
{
	EG_CORE_ASSERT(rb != nullptr, "radio button is nullptr");

	if (data.contains("OnMouseMoved"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseMoved"].is_string(), "RadioButton control with name '{}': 'OnMouseMoved' value must be a string. Invalid RadioButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseMoved"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<RadioButton, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RadioButton control with name '{}': 'OnMouseMoved' callback not found for key '{}'. Invalid RadioButton object: {}", m_name, key, data.dump(4));
		rb->SetOnMouseMovedCallback(callback);
	}
}
void RadioButtonLoader::ParseOnMouseLButtonDown(RadioButton* rb, json& data)
{
	EG_CORE_ASSERT(rb != nullptr, "radio button is nullptr");

	if (data.contains("OnMouseLButtonDown"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLButtonDown"].is_string(), "RadioButton control with name '{}': 'OnMouseLButtonDown' value must be a string. Invalid RadioButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseLButtonDown"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<RadioButton, MouseButtonPressedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RadioButton control with name '{}': 'OnMouseLButtonDown' callback not found for key '{}'. Invalid RadioButton object: {}", m_name, key, data.dump(4));
		rb->SetOnMouseLButtonDownCallback(callback);
	}
}
void RadioButtonLoader::ParseOnMouseLButtonUp(RadioButton* rb, json& data)
{
	EG_CORE_ASSERT(rb != nullptr, "radio button is nullptr");

	if (data.contains("OnMouseLButtonUp"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLButtonUp"].is_string(), "RadioButton control with name '{}': 'OnMouseLButtonUp' value must be a string. Invalid RadioButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseLButtonUp"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<RadioButton, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RadioButton control with name '{}': 'OnMouseLButtonUp' callback not found for key '{}'. Invalid RadioButton object: {}", m_name, key, data.dump(4));
		rb->SetOnMouseLButtonUpCallback(callback);
	}
}
void RadioButtonLoader::ParseOnIsCheckedChanged(RadioButton* rb, json& data)
{
	EG_CORE_ASSERT(rb != nullptr, "radio button is nullptr");

	if (data.contains("OnIsCheckedChanged"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnIsCheckedChanged"].is_string(), "RadioButton control with name '{}': 'OnIsCheckedChanged' value must be a string. Invalid RadioButton object: {}", m_name, data.dump(4));
		std::string key = data["OnIsCheckedChanged"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<RadioButton, RadioButtonIsCheckedChangedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RadioButton control with name '{}': 'OnIsCheckedChanged' callback not found for key '{}'. Invalid RadioButton object: {}", m_name, key, data.dump(4));
		rb->SetOnIsCheckedChanged(callback);
	}
}

}