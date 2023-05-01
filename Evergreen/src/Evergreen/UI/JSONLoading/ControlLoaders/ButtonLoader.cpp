#include "pch.h"
#include "ButtonLoader.h"



namespace Evergreen
{
Control* ButtonLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, 
	std::optional<RowColumnPosition> rowColumnPositionOverride)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique

	RowColumnPosition rowCol;
	Margin margin;

	// Parse Row/Column
	rowCol = rowColumnPositionOverride.has_value() ? rowColumnPositionOverride.value() : ParseRowColumnPosition(data);

	// Parse Margin
	margin = ParseMargin(data);

	// Parse Brushes
	std::unique_ptr<ColorBrush> backgroundBrush = ParseBackgroundBrush(deviceResources, data);
	EG_CORE_ASSERT(backgroundBrush != nullptr, "Not allowed to return nullptr. Should have thrown exception");

	//       Note: this function IS allowed to be nullptr and if so, the Button will just create a default brush
	std::unique_ptr<ColorBrush> borderBrush = ParseBorderBrush(deviceResources, data);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Text", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"BackgroundBrush", "BorderBrush", "BorderWidth", "Content", "OnMouseEnter", "OnMouseLeave", "OnMouseMoved",
	"OnMouseLButtonDown", "OnClick", "OnUpdate", "BorderTopLeftOffsetX", "BorderTopLeftOffsetY",
	"BorderTopRightOffsetX", "BorderTopRightOffsetY", "BorderBottomLeftOffsetX", "BorderBottomLeftOffsetY",
	"BorderBottomRightOffsetX", "BorderBottomRightOffsetY", "CornerRadiusX", "CornerRadiusY", "CornerRadius" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Button control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Button control
	Button* button = parent->CreateControl<Button>(rowCol, deviceResources, std::move(backgroundBrush), std::move(borderBrush), margin);
	EG_CORE_ASSERT(button != nullptr, "Something went wrong, button is nullptr");

	button->Name(name);
	button->ID(ParseID(data));

	ParseContent(deviceResources, button->GetLayout(), data);

	// Border Width
	ParseBorderWidth(button, data);
	ParseBorderOffsets(button, data);
	ParseCornerRadius(button, data);

	ParseOnMouseEntered(button, data);
	ParseOnMouseExited(button, data);
	ParseOnMouseMoved(button, data);
	ParseOnMouseLButtonDown(button, data);
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

	// Not including a 'BorderBrush' key is only invalid if the 'BorderWidth' key is not specified
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BorderBrush"), "Button control with name '{}': When 'BorderWidth' key is present, the 'BorderBrush' key is also required. Incomplete Button object: {}", m_name, data.dump(4));
	}

	if (data.contains("BorderBrush"))
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"]));

	// Note: we are allowed to return nullptr here because the Button class will just create a default border brush
	return nullptr;
}
void ButtonLoader::ParseBorderWidth(Button* button, json& data)
{
	if (data.contains("BorderWidth"))
	{
		if (data["BorderWidth"].is_number())
		{
			float width = data["BorderWidth"].get<float>();
			JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "Button control with name '{}': 'BorderWidth' is not allowed to be less than 0. Invalid Button object: {}", m_name, data.dump(4));
			button->BorderWidth(width);
		}
		else if (data["BorderWidth"].is_array())
		{
			std::array<float, 4> borderWidths{ 0.0f, 0.0f, 0.0f, 0.0f };
			JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].size() == 4, "Button control with name '{}': When specificying 'BorderWidth' as an array of floats, there must be exactly 4 values in the array. Invalid Button object: {}", m_name, data.dump(4));

			float width = 0.0f;
			for (unsigned int iii = 0; iii < 4; ++iii)
			{
				JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"][iii].is_number(), "Button control with name '{}': When specificying 'BorderWidth' as an array, the array values must be parsable numbers. Invalid Button object: {}", m_name, data.dump(4));
				width = data["BorderWidth"][iii].get<float>();
				JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "Button control with name '{}': 'BorderWidth' array values are not allowed to be less than 0. Invalid Button object: {}", m_name, data.dump(4));
				borderWidths[iii] = width;
			}

			button->BorderWidth(borderWidths);
		}
		else
		{
			JSON_LOADER_EXCEPTION("Button control with name '{}': 'BorderWidth' value must either be a number of an array of 4 floats. Invalid Button object: {}", m_name, data.dump(4));
		}
	}
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

void ButtonLoader::ParseBorderOffsets(Button* button, json& data)
{
	EG_CORE_ASSERT(button != nullptr, "button is nullptr");

	if (data.contains("BorderTopLeftOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopLeftOffsetX"].is_number(), "Button control with name '{}': 'BorderTopLeftOffsetX' value must be a number. Invalid Button object: {}", m_name, data.dump(4));
		float value = data["BorderTopLeftOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Button control with name '{}': 'BorderTopLeftOffsetX' value must be >= 0. Invalid Button object: {}", m_name, data.dump(4));
		button->BorderTopLeftOffsetX(value);
	}
	if (data.contains("BorderTopLeftOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopLeftOffsetY"].is_number(), "Button control with name '{}': 'BorderTopLeftOffsetY' value must be a number. Invalid Button object: {}", m_name, data.dump(4));
		float value = data["BorderTopLeftOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Button control with name '{}': 'BorderTopLeftOffsetY' value must be >= 0. Invalid Button object: {}", m_name, data.dump(4));
		button->BorderTopLeftOffsetY(value);
	}
	if (data.contains("BorderTopRightOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopRightOffsetX"].is_number(), "Button control with name '{}': 'BorderTopRightOffsetX' value must be a number. Invalid Button object: {}", m_name, data.dump(4));
		float value = data["BorderTopRightOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Button control with name '{}': 'BorderTopRightOffsetX' value must be >= 0. Invalid Button object: {}", m_name, data.dump(4));
		button->BorderTopRightOffsetX(value);
	}
	if (data.contains("BorderTopRightOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopRightOffsetY"].is_number(), "Button control with name '{}': 'BorderTopRightOffsetY' value must be a number. Invalid Button object: {}", m_name, data.dump(4));
		float value = data["BorderTopRightOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Button control with name '{}': 'BorderTopRightOffsetY' value must be >= 0. Invalid Button object: {}", m_name, data.dump(4));
		button->BorderTopRightOffsetY(value);
	}
	if (data.contains("BorderBottomLeftOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomLeftOffsetX"].is_number(), "Button control with name '{}': 'BorderBottomLeftOffsetX' value must be a number. Invalid Button object: {}", m_name, data.dump(4));
		float value = data["BorderBottomLeftOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Button control with name '{}': 'BorderBottomLeftOffsetX' value must be >= 0. Invalid Button object: {}", m_name, data.dump(4));
		button->BorderBottomLeftOffsetX(value);
	}
	if (data.contains("BorderBottomLeftOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomLeftOffsetY"].is_number(), "Button control with name '{}': 'BorderBottomLeftOffsetY' value must be a number. Invalid Button object: {}", m_name, data.dump(4));
		float value = data["BorderBottomLeftOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Button control with name '{}': 'BorderBottomLeftOffsetY' value must be >= 0. Invalid Button object: {}", m_name, data.dump(4));
		button->BorderBottomLeftOffsetY(value);
	}
	if (data.contains("BorderBottomRightOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomRightOffsetX"].is_number(), "Button control with name '{}': 'BorderBottomRightOffsetX' value must be a number. Invalid Button object: {}", m_name, data.dump(4));
		float value = data["BorderBottomRightOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Button control with name '{}': 'BorderBottomRightOffsetX' value must be >= 0. Invalid Button object: {}", m_name, data.dump(4));
		button->BorderBottomRightOffsetX(value);
	}
	if (data.contains("BorderBottomRightOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomRightOffsetY"].is_number(), "Button control with name '{}': 'BorderBottomRightOffsetY' value must be a number. Invalid Button object: {}", m_name, data.dump(4));
		float value = data["BorderBottomRightOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Button control with name '{}': 'BorderBottomRightOffsetY' value must be >= 0. Invalid Button object: {}", m_name, data.dump(4));
		button->BorderBottomRightOffsetY(value);
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

void ButtonLoader::ParseCornerRadius(Button* button, json& data)
{
	if (data.contains("CornerRadius"))
	{
		if (data.contains("CornerRadiusX"))
		{
			EG_CORE_WARN("Button control with name '{}': Ignoring key 'CornerRadiusX' because key 'CornerRadius' was found and takes precendent. You should either only use 'CornerRadius' or use BOTH 'CornerRadiusX' and 'CornerRadiusY'.", m_name);
		}
		if (data.contains("CornerRadiusY"))
		{
			EG_CORE_WARN("Button control with name '{}': Ignoring key 'CornerRadiusY' because key 'CornerRadius' was found and takes precendent. You should either only use 'CornerRadius' or use BOTH 'CornerRadiusX' and 'CornerRadiusY'.", m_name);
		}

		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadius"].is_number(), "Button control with name '{}': 'CornerRadius' value must be a number. Invalid Button object: {}", m_name, data.dump(4));

		float radius = data["CornerRadius"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(radius >= 0, "Button control with name '{}': 'CornerRadius' is not allowed to be less than 0. Invalid Button object: {}", m_name, data.dump(4));

		button->SetCornerRadius(radius);
	}
	else
	{
		if (data.contains("CornerRadiusX"))
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadiusX"].is_number(), "Button control with name '{}': 'CornerRadiusX' value must be a number. Invalid Button object: {}", m_name, data.dump(4));

			float radiusX = data["CornerRadiusX"].get<float>();

			JSON_LOADER_EXCEPTION_IF_FALSE(radiusX >= 0, "Button control with name '{}': 'CornerRadiusX' is not allowed to be less than 0. Invalid Button object: {}", m_name, data.dump(4));

			button->SetCornerRadiusX(radiusX);
		}

		if (data.contains("CornerRadiusY"))
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadiusY"].is_number(), "Button control with name '{}': 'CornerRadiusX' value must be a number. Invalid Button object: {}", m_name, data.dump(4));

			float radiusY = data["CornerRadiusY"].get<float>();

			JSON_LOADER_EXCEPTION_IF_FALSE(radiusY >= 0, "Button control with name '{}': 'CornerRadiusY' is not allowed to be less than 0. Invalid Button object: {}", m_name, data.dump(4));

			button->SetCornerRadiusY(radiusY);
		}
	}
}


}