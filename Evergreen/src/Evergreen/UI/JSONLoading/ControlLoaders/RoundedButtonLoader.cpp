#include "pch.h"
#include "RoundedButtonLoader.h"



namespace Evergreen
{
Control* RoundedButtonLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name)
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

	// Parse Brushes
	std::unique_ptr<ColorBrush> backgroundBrush = ParseBackgroundBrush(deviceResources, data);
	EG_CORE_ASSERT(backgroundBrush != nullptr, "Not allowed to return nullptr. Should have thrown exception");

	//       Note: this function IS allowed to be nullptr and if so, the Button will just create a default brush
	std::unique_ptr<ColorBrush> borderBrush = ParseBorderBrush(deviceResources, data);

	// Border Width
	float borderWidth = ParseBorderWidth(data);

	// Corner radius
	float radiusX = ParseRadiusX(data);
	float radiusY = ParseRadiusY(data);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Text", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"BackgroundBrush", "BorderBrush", "BorderWidth", "CornerRadiusX", "CornerRadiusY", "Content", "OnMouseEnter",
	"OnMouseMoved", "OnMouseLeave", "OnMouseLButtonDown", "OnMouseLButtonUp", "OnClick", "OnUpdate"};
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - RoundedButton control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	RoundedButton* button = parent->CreateControl<RoundedButton>(rowCol, deviceResources, std::move(backgroundBrush), std::move(borderBrush), radiusX, radiusY, borderWidth, margin);
	EG_CORE_ASSERT(button != nullptr, "Something went wrong, button is nullptr");

	button->Name(name);
	button->ID(ParseID(data));

	ParseContent(deviceResources, button->GetLayout(), data);

	ParseOnMouseEntered(button, data);
	ParseOnMouseExited(button, data);
	ParseOnMouseLButtonDown(button, data);
	ParseOnMouseLButtonUp(button, data);
	ParseOnClick(button, data);

	ParseOnUpdateCallback(button, data);

	return button;
}

std::unique_ptr<ColorBrush> RoundedButtonLoader::ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BackgroundBrush"), "RoundedButton control with name '{}': No 'BackgroundBrush' key. Incomplete RoundedButton object: {}", m_name, data.dump(4));

	return std::move(JSONLoaders::LoadBrush(deviceResources, data["BackgroundBrush"]));
}
std::unique_ptr<ColorBrush> RoundedButtonLoader::ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// Not including a 'BorderBrush' key is only invalid if a 'BorderWidth' greater than 0 has been specified
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].is_number(), "RoundedButton control with name '{}': Parsing BorderBrush, but noticed that 'BorderWidth' value is not a number. Invalid RoundedButton object: {}", m_name, data.dump(4));

		float borderWidth = data["BorderWidth"].get<float>();
		if (borderWidth > 0.0f)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BorderBrush"), "RoundedButton control with name '{}': 'BorderWidth' is non-zero, but no 'BorderBrush' has been specified. Incomplete RoundedButton object: {}", m_name, data.dump(4));
		}
	}

	if (data.contains("BorderBrush"))
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"]));

	// Note: we are allowed to return nullptr here because the Button class will just create a default border brush
	return nullptr;
}
float RoundedButtonLoader::ParseBorderWidth(json& data)
{
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].is_number(), "RoundedButton control with name '{}': 'BorderWidth' value must be a number. Invalid RoundedButton object: {}", m_name, data.dump(4));

		float borderWidth = data["BorderWidth"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(borderWidth >= 0, "RoundedButton control with name '{}': 'BorderWidth' is not allowed to be less than 0. Incomplete RoundedButton object: {}", m_name, data.dump(4));

		return borderWidth;
	}

	return 0.0f;
}
float RoundedButtonLoader::ParseRadiusX(json& data)
{
	if (data.contains("CornerRadiusX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadiusX"].is_number(), "RoundedButton control with name '{}': 'CornerRadiusX' value must be a number. Invalid RoundedButton object: {}", m_name, data.dump(4));

		float radiusX = data["CornerRadiusX"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(radiusX >= 0, "RoundedButton control with name '{}': 'CornerRadiusX' is not allowed to be less than 0. Incomplete RoundedButton object: {}", m_name, data.dump(4));

		return radiusX;
	}

	return 0.0f;
}
float RoundedButtonLoader::ParseRadiusY(json& data)
{
	if (data.contains("CornerRadiusY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadiusY"].is_number(), "RoundedButton control with name '{}': 'CornerRadiusX' value must be a number. Invalid RoundedButton object: {}", m_name, data.dump(4));

		float radiusY = data["CornerRadiusY"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(radiusY >= 0, "RoundedButton control with name '{}': 'CornerRadiusY' is not allowed to be less than 0. Incomplete RoundedButton object: {}", m_name, data.dump(4));

		return radiusY;
	}

	return 0.0f;
}
void RoundedButtonLoader::ParseContent(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "layout is nullptr");

	// If no content is given, just leave the layout empty, but in order for the layout to be valid, there must be at least one row and column
	if (!data.contains("Content"))
	{
		layout->AddRow({ RowColumnType::STAR, 1.0f });
		layout->AddColumn({ RowColumnType::STAR, 1.0f });
		return;
	}

	JSON_LOADER_EXCEPTION_IF_FALSE(data["Content"].is_object(), "RoundedButton control with name '{}': 'Content' value must be a json object. Invalid RoundedButton object: {}", m_name, data.dump(4));

	json content = data["Content"];

	JSON_LOADER_EXCEPTION_IF_FALSE(content.contains("Type"), "RoundedButton control with name '{}': 'Content' json object must contain key 'Type'. Invalid RoundedButton object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(content["Type"].is_string(), "RoundedButton control with name '{}': 'Content' json object key 'Type' must have a string value. Invalid RoundedButton object: {}", m_name, data.dump(4));

	std::string contentType = content["Type"].get<std::string>();

	if (contentType.compare("Text") == 0)
	{
		// We are only loading a single text object into the layout, so create a default row and default column then add the Text control
		layout->AddRow({ RowColumnType::STAR, 1.0f });
		layout->AddColumn({ RowColumnType::STAR, 1.0f });

		JSONLoaders::LoadControl(deviceResources, "Text", layout, content, m_name + "_Text");
	}
	else if (contentType.compare("Layout") == 0)
	{
		// We require the the layout use the same brush as the button itself
		if (content.contains("Brush"))
		{
			EG_CORE_WARN("{}:{} - RoundedButton control with name '{}': 'Content' layout object should not contain 'Brush' keyword. Ignoring 'Brush' value for json object: {}", __FILE__, __LINE__, m_name, data.dump(4));
		}
		content["Brush"] = data["BackgroundBrush"];
		JSONLoaders::LoadLayout(deviceResources, layout, content);
	}
	else
	{
		JSON_LOADER_EXCEPTION("RoundedButton control with name '{}': 'Content' json object key 'Type' value ('{}') was not recoginized - value must either be 'Text' or 'Layout'. Invalid RoundedButton object: {}", m_name, contentType, data.dump(4));
	}
}

void RoundedButtonLoader::ParseOnMouseEntered(RoundedButton* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseEnter"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseEnter"].is_string(), "RoundedButton control with name '{}': 'OnMouseEnter' value must be a string. Invalid RoundedButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseEnter"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RoundedButton control with name '{}': 'OnMouseEnter' callback not found for key '{}'. Invalid RoundedButton object: {}", m_name, key, data.dump(4));
		button->SetOnMouseEnteredCallback(callback);
	}
}
void RoundedButtonLoader::ParseOnMouseExited(RoundedButton* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseLeave"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLeave"].is_string(), "RoundedButton control with name '{}': 'OnMouseLeave' value must be a string. Invalid RoundedButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseLeave"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RoundedButton control with name '{}': 'OnMouseLeave' callback not found for key '{}'. Invalid RoundedButton object: {}", m_name, key, data.dump(4));
		button->SetOnMouseExitedCallback(callback);
	}
}
void RoundedButtonLoader::ParseOnMouseMoved(RoundedButton* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseMoved"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseMoved"].is_string(), "RoundedButton control with name '{}': 'OnMouseMoved' value must be a string. Invalid RoundedButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseMoved"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RoundedButton control with name '{}': 'OnMouseMoved' callback not found for key '{}'. Invalid RoundedButton object: {}", m_name, key, data.dump(4));
		button->SetOnMouseMovedCallback(callback);
	}
}
void RoundedButtonLoader::ParseOnMouseLButtonDown(RoundedButton* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseLButtonDown"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLButtonDown"].is_string(), "RoundedButton control with name '{}': 'OnMouseLButtonDown' value must be a string. Invalid RoundedButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseLButtonDown"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseButtonPressedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RoundedButton control with name '{}': 'OnMouseLButtonDown' callback not found for key '{}'. Invalid RoundedButton object: {}", m_name, key, data.dump(4));
		button->SetOnMouseLButtonDownCallback(callback);
	}
}
void RoundedButtonLoader::ParseOnMouseLButtonUp(RoundedButton* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseLButtonUp"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLButtonUp"].is_string(), "RoundedButton control with name '{}': 'OnMouseLButtonUp' value must be a string. Invalid RoundedButton object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseLButtonUp"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RoundedButton control with name '{}': 'OnMouseLButtonUp' callback not found for key '{}'. Invalid RoundedButton object: {}", m_name, key, data.dump(4));
		button->SetOnMouseLButtonUpCallback(callback);
	}
}
void RoundedButtonLoader::ParseOnClick(RoundedButton* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnClick"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnClick"].is_string(), "RoundedButton control with name '{}': 'OnClick' value must be a string. Invalid RoundedButton object: {}", m_name, data.dump(4));
		std::string key = data["OnClick"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "RoundedButton control with name '{}': 'OnClick' callback not found for key '{}'. Invalid RoundedButton object: {}", m_name, key, data.dump(4));
		button->SetOnClickCallback(callback);
	}
}


}