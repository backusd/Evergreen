#include "pch.h"
#include "ButtonLoader.h"



namespace Evergreen
{
Control* ButtonLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name)
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

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Text", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"BackgroundBrush", "BorderBrush", "BorderWidth", "Content", "OnMouseEnter", "OnMouseLeave", "OnMouseMoved",
	"OnMouseLButtonDown", "OnMouseLButtonUp", "OnClick", "OnUpdate"};
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Button control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	Button* button = parent->CreateControl<Button>(rowCol, deviceResources, std::move(backgroundBrush), std::move(borderBrush), borderWidth, margin);
	EG_CORE_ASSERT(button != nullptr, "Something went wrong, button is nullptr");

	button->Name(name);
	button->ID(ParseID(data));

	ParseContent(deviceResources, button->GetLayout(), data);

	ParseOnMouseEntered(button, data);
	ParseOnMouseExited(button, data);
	ParseOnMouseMoved(button, data);
	ParseOnMouseLButtonDown(button, data);
	ParseOnMouseLButtonUp(button, data);
	ParseOnClick(button, data);

	ParseOnUpdateCallback(button, data);

	return button;
}

std::unique_ptr<ColorBrush> ButtonLoader::ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BackgroundBrush"), "Button control with name '{}': No 'BackgroundBrush' key. Incomplete Button object: {}", m_name, data.dump(4));

	return std::move(JSONLoaders::LoadBrush(deviceResources, data["BackgroundBrush"]));
}
std::unique_ptr<ColorBrush> ButtonLoader::ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// Not including a 'BorderBrush' key is only invalid if a 'BorderWidth' greater than 0 has been specified
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].is_number(), "Button control with name '{}': Parsing BorderBrush, but noticed that 'BorderWidth' value is not a number. Invalid Button object: {}", m_name, data.dump(4));

		float borderWidth = data["BorderWidth"].get<float>();
		if (borderWidth > 0.0f)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BorderBrush"), "Button control with name '{}': 'BorderWidth' is non-zero, but no 'BorderBrush' has been specified. Incomplete Button object: {}", m_name, data.dump(4));
		}
	}

	if (data.contains("BorderBrush"))
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"]));

	// Note: we are allowed to return nullptr here because the Button class will just create a default border brush
	return nullptr;
}
float ButtonLoader::ParseBorderWidth(json& data)
{
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].is_number(), "Button control with name '{}': 'BorderWidth' value must be a number. Invalid Button object: {}", m_name, data.dump(4));

		float borderWidth = data["BorderWidth"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(borderWidth >= 0.0f, "Button control with name '{}': 'BorderWidth' is not allowed to be less than 0. Incomplete Button object: {}", m_name, data.dump(4));
		
		return borderWidth;
	}

	return 0.0f;
}
void ButtonLoader::ParseContent(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "layout is nullptr");

	// If no content is given, just leave the layout empty, but in order for the layout to be valid, there must be at least one row and column
	if (!data.contains("Content"))
	{
		layout->AddRow({ RowColumnType::STAR, 1.0f });
		layout->AddColumn({ RowColumnType::STAR, 1.0f });
		return;
	}

	JSON_LOADER_EXCEPTION_IF_FALSE(data["Content"].is_object(), "Button control with name '{}': 'Content' value must be a json object. Invalid Button object: {}", m_name, data.dump(4));

	json content = data["Content"];

	JSON_LOADER_EXCEPTION_IF_FALSE(content.contains("Type"), "Button control with name '{}': 'Content' json object must contain key 'Type'. Invalid Button object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(content["Type"].is_string(), "Button control with name '{}': 'Content' json object key 'Type' must have a string value. Invalid Button object: {}", m_name, data.dump(4));

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
		JSONLoaders::LoadLayout(deviceResources, layout, content);
	}
	else
	{
		JSON_LOADER_EXCEPTION("Button control with name '{}': 'Content' json object key 'Type' value ('{}') was not recoginized - value must either be 'Text' or 'Layout'. Invalid Button object: {}", m_name, contentType, data.dump(4));
	}
}

void ButtonLoader::ParseOnMouseEntered(Button* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseEnter"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseEnter"].is_string(), "Button control with name '{}': 'OnMouseEnter' value must be a string. Invalid Button object: {}", m_name, data.dump(4));

		std::string key = data["OnMouseEnter"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Button control with name '{}': 'OnMouseEnter' callback not found for key '{}'. Invalid Button object: {}", m_name, key, data.dump(4));
		button->SetOnMouseEnteredCallback(callback);
	}
}
void ButtonLoader::ParseOnMouseExited(Button* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseLeave"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLeave"].is_string(), "Button control with name '{}': 'OnMouseLeave' value must be a string. Invalid Button object: {}", m_name, data.dump(4));

		std::string key = data["OnMouseLeave"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Button control with name '{}': 'OnMouseLeave' callback not found for key '{}'. Invalid Button object: {}", m_name, key, data.dump(4));
		button->SetOnMouseExitedCallback(callback);
	}
}
void ButtonLoader::ParseOnMouseMoved(Button* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseMoved"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseMoved"].is_string(), "Button control with name '{}': 'OnMouseMoved' value must be a string. Invalid Button object: {}", m_name, data.dump(4));

		std::string key = data["OnMouseMoved"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Button control with name '{}': 'OnMouseMoved' callback not found for key '{}'. Invalid Button object: {}", m_name, key, data.dump(4));
		button->SetOnMouseMovedCallback(callback);
	}
}
void ButtonLoader::ParseOnMouseLButtonDown(Button* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseLButtonDown"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLButtonDown"].is_string(), "Button control with name '{}': 'OnMouseLButtonDown' value must be a string. Invalid Button object: {}", m_name, data.dump(4));

		std::string key = data["OnMouseLButtonDown"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseButtonPressedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Button control with name '{}': 'OnMouseLButtonDown' callback not found for key '{}'. Invalid Button object: {}", m_name, key, data.dump(4));
		button->SetOnMouseLButtonDownCallback(callback);
	}
}
void ButtonLoader::ParseOnMouseLButtonUp(Button* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnMouseLButtonUp"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLButtonUp"].is_string(), "Button control with name '{}': 'OnMouseLButtonUp' value must be a string. Invalid Button object: {}", m_name, data.dump(4));

		std::string key = data["OnMouseLButtonUp"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Button control with name '{}': 'OnMouseLButtonUp' callback not found for key '{}'. Invalid Button object: {}", m_name, key, data.dump(4));
		button->SetOnMouseLButtonUpCallback(callback);
	}
}
void ButtonLoader::ParseOnClick(Button* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("OnClick"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnClick"].is_string(), "Button control with name '{}': 'OnClick' value must be a string. Invalid Button object: {}", m_name, data.dump(4));

		std::string key = data["OnClick"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<Button, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Button control with name '{}': 'OnClick' callback not found for key '{}'. Invalid Button object: {}", m_name, key, data.dump(4));
		button->SetOnClickCallback(callback);
	}
}


}