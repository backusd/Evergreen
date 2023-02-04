#include "pch.h"
#include "PaneLoader.h"
#include "Evergreen/UI/UI.h"

namespace Evergreen
{
Control* PaneLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	// NOTE: The pane will not be created as a child control of the parent layout. However, in order to keep the 
	// same function signature, we include a Layout* parameter. We are also going to require it not be nullptr
	// so that we can get the UI* which is how we will end up storing the created Pane
	EG_CORE_ASSERT(parent != nullptr, "No parent layout"); 
	UI* ui = parent->GetUI();
	EG_CORE_ASSERT(ui != nullptr, "No UI");

	m_name = name;

	// Pane is special in that row/column positioning and margin doesn't make sense
	// So if the user included them, just log a warning message and delete that piece of data
	LogWarningForRowColumnPositionAndMarginKeywords(data);

	float top = ParseTop(data);
	float left = ParseLeft(data);
	float height = ParseHeight(data);
	float width = ParseWidth(data);

	bool resizable = ParseResizable(data);
	bool relocatable = ParseRelocatable(data);
	
	// Parse Brushes
	std::unique_ptr<ColorBrush> backgroundBrush = ParseBackgroundBrush(deviceResources, data);
	std::unique_ptr<ColorBrush> borderBrush = ParseBorderBrush(deviceResources, data);

	// Border Width
	float borderWidth = ParseBorderWidth(data);

	// Title attributes
	bool includeTitleBar = ParseIncludeTitleBar(data);
	std::unique_ptr<ColorBrush> titleBarBrush = ParseTitleBarBrush(deviceResources, data);
	float titleBarHeight = ParseTitleBarHeight(data);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Text", "Title", "Top", "Left", "Height", "Width", "Resizable",
	"Relocatable", "BackgroundBrush", "BorderBrush", "BorderWidth", "CornerRadius", "CornerRadiusX", "CornerRadiusY", 
	"IncludeTitleBar", "TitleBarBrush", "TitleBarHeight", "IsMinimized", "IsVisible", "Content", "OnMouseEnteredTitleBar", 
	"OnMouseExitedTitleBar", "OnMouseEnteredContentRegion", "OnMouseExitedContentRegion", "OnMouseMoved" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Pane control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	std::unique_ptr<Pane> p = std::make_unique<Pane>(
		deviceResources,
		ui, // UI*
		top, // top
		left, // left
		height, // height
		width, // width
		resizable, // resizable 
		relocatable, // relocatable
		std::move(backgroundBrush), // background brush
		std::move(borderBrush), // border brush
		borderWidth, // border width
		includeTitleBar, // includeTitleBar
		std::move(titleBarBrush), // TitleBarBrush
		titleBarHeight // TitleBar height
		);
	Pane* pane = ui->AddPane(std::move(p), name);
	EG_CORE_ASSERT(pane != nullptr, "Something went wrong, button is nullptr");

	pane->Name(name);
	pane->ID(ParseID(data));

	ParseCornerRadius(pane, data);
	ParseIsMinimized(pane, data);
	ParseIsVisible(pane, data);

	ParseTitle(pane, data);
	ParseContent(pane, data);

	ParseOnMouseEnteredTitleBar(pane, data);
	ParseOnMouseExitedTitleBar(pane, data);
	ParseOnMouseEnteredContentRegion(pane, data);
	ParseOnMouseExitedContentRegion(pane, data);
	ParseOnMouseMoved(pane, data);

	return pane;
}

void PaneLoader::LogWarningForRowColumnPositionAndMarginKeywords(json& data)
{
	constexpr std::array keysToIgnore{ "Row", "Column", "RowSpan", "ColumnSpane", "Margin"};
	for (const std::string& s : keysToIgnore)
	{
		if (data.contains(s))
		{
			EG_CORE_WARN("Ignoring key '{}' in Pane with name '{}'. Row/Column Positioning and Margin keywords do not make sense for Panes", s, m_name);
			data.erase(s);
		}
	}
}

float PaneLoader::ParseTop(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Top"), "Pane control with name '{}': 'Top' key is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Top"].is_number(), "Pane control with name '{}': 'Top' field must be a number. Invalid Pane object: {}", m_name, data.dump(4));

	float top = data["Top"].get<float>();
	JSON_LOADER_EXCEPTION_IF_FALSE(top > 0.0f, "Pane control with name '{}': 'Top' value must be greater than 0.0f. Invalid Pane object: {}", m_name, data.dump(4));

	return top;
}
float PaneLoader::ParseLeft(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Left"), "Pane control with name '{}': 'Left' key is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Left"].is_number(), "Pane control with name '{}': 'Left' field must be a number. Invalid Pane object: {}", m_name, data.dump(4));

	float left = data["Left"].get<float>();
	JSON_LOADER_EXCEPTION_IF_FALSE(left > 0.0f, "Pane control with name '{}': 'Left' value must be greater than 0.0f. Invalid Pane object: {}", m_name, data.dump(4));

	return left;
}
float PaneLoader::ParseHeight(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Height"), "Pane control with name '{}': 'Height' key is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Height"].is_number(), "Pane control with name '{}': 'Height' field must be a number. Invalid Pane object: {}", m_name, data.dump(4));

	float height = data["Height"].get<float>();
	JSON_LOADER_EXCEPTION_IF_FALSE(height > 0.0f, "Pane control with name '{}': 'Height' value must be greater than 0.0f. Invalid Pane object: {}", m_name, data.dump(4));

	return height;
}
float PaneLoader::ParseWidth(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Width"), "Pane control with name '{}': 'Width' key is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Width"].is_number(), "Pane control with name '{}': 'Width' field must be a number. Invalid Pane object: {}", m_name, data.dump(4));

	float width = data["Width"].get<float>();
	JSON_LOADER_EXCEPTION_IF_FALSE(width > 0.0f, "Pane control with name '{}': 'Width' value must be greater than 0.0f. Invalid Pane object: {}", m_name, data.dump(4));

	return width;
}

bool PaneLoader::ParseResizable(json& data)
{
	if (data.contains("Resizable"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Resizable"].is_boolean(), "Pane control with name '{}': 'Resizable' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		return data["Resizable"].get<bool>();
	}
	return true;
}
bool PaneLoader::ParseRelocatable(json& data)
{
	if (data.contains("Relocatable"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Relocatable"].is_boolean(), "Pane control with name '{}': 'Relocatable' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		return data["Relocatable"].get<bool>();
	}
	return true;
}

std::unique_ptr<ColorBrush> PaneLoader::ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (data.contains("BackgroundBrush"))
	{
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BackgroundBrush"]));
	}

	return nullptr;
}
std::unique_ptr<ColorBrush> PaneLoader::ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (data.contains("BorderBrush"))
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"]));

	// Note: we are allowed to return nullptr here because the Pane class will just create a default border brush
	return nullptr;
}
float PaneLoader::ParseBorderWidth(json& data)
{
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].is_number(), "Pane control with name '{}': 'BorderWidth' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));

		float borderWidth = data["BorderWidth"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(borderWidth >= 0.0f, "Pane control with name '{}': 'BorderWidth' is not allowed to be less than 0. Invalid Pane object: {}", m_name, data.dump(4));

		return borderWidth;
	}

	return 0.0f;
}

bool PaneLoader::ParseIncludeTitleBar(json& data)
{
	if (data.contains("IncludeTitleBar"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["IncludeTitleBar"].is_boolean(), "Pane control with name '{}': 'IncludeTitleBar' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		return data["IncludeTitleBar"].get<bool>();
	}
	return true;
}
std::unique_ptr<ColorBrush> PaneLoader::ParseTitleBarBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (data.contains("TitleBarBrush"))
	{
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["TitleBarBrush"]));
	}

	return nullptr;
}
float PaneLoader::ParseTitleBarHeight(json& data)
{
	if (data.contains("TitleBarHeight"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["TitleBarHeight"].is_number(), "Pane control with name '{}': 'TitleBarHeight' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));

		float height = data["TitleBarHeight"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(height > 0.0f, "Pane control with name '{}': 'TitleBarHeight' must be greater than 0. Invalid Pane object: {}", m_name, data.dump(4));

		return height;
	}

	return 20.0f;
}

void PaneLoader::ParseCornerRadius(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	if (data.contains("CornerRadius"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("CornerRadiusX"), "Pane control with name '{}': When 'CornerRadius' field is used, it is invalid to also include 'CornerRadiusX'. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("CornerRadiusY"), "Pane control with name '{}': When 'CornerRadius' field is used, it is invalid to also include 'CornerRadiusY'. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadius"].is_number(), "Pane control with name '{}': 'CornerRadius' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float radius = data["CornerRadius"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radius >= 0.0f, "Pane control with name '{}': 'CornerRadius' must be greater than or equal to 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->SetCornerRadius(radius);
	}
	else if (data.contains("CornerRadiusX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("CornerRadiusY"), "Pane control with name '{}': When 'CornerRadiusX' field is used, 'CornerRadiusY' is also required. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadiusX"].is_number(), "Pane control with name '{}': 'CornerRadiusX' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadiusY"].is_number(), "Pane control with name '{}': 'CornerRadiusY' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float radiusX = data["CornerRadiusX"].get<float>();
		float radiusY = data["CornerRadiusY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radiusX >= 0.0f, "Pane control with name '{}': 'CornerRadiusX' must be greater than or equal to 0. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(radiusY >= 0.0f, "Pane control with name '{}': 'CornerRadiusY' must be greater than or equal to 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->SetCornerRadius(radiusX, radiusY);
	}
	else
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("CornerRadiusY"), "Pane control with name '{}': When 'CornerRadiusY' field is used, 'CornerRadiusX' is also required. Invalid Pane object: {}", m_name, data.dump(4));
	}
}
void PaneLoader::ParseIsMinimized(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");
	if (data.contains("IsMinimized"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["IsMinimized"].is_boolean(), "Pane control with name '{}': 'IsMinimized' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		pane->SetMinimized(data["IsMinimized"].get<bool>());
	}
}
void PaneLoader::ParseIsVisible(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");
	if (data.contains("IsVisible"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["IsVisible"].is_boolean(), "Pane control with name '{}': 'IsVisible' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		pane->SetVisible(data["IsVisible"].get<bool>());
	}
}
void PaneLoader::ParseTitle(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	bool includeTitleBar = ParseIncludeTitleBar(data);
	if (!includeTitleBar)
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("Title"), "Pane control with name '{}': When 'IncludeTitleBar' field is false, it is invalid to also include a 'Title' field. Invalid Pane object: {}", m_name, data.dump(4));
		return;
	}

	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Title"), "Pane control with name '{}': When 'IncludeTitleBar' field is true, a 'Title' field must also be present. Invalid Pane object: {}", m_name, data.dump(4));

	if (data["Title"].is_string())
	{
		pane->ClearTitleBarLayoutAndAddTitle(data["Title"].get<std::string>());
	}
	else
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Title"].is_object(), "Pane control with name '{}': 'Title' field must either be a string or a json object that represents a Layout. Invalid Pane object: {}", m_name, data.dump(4));
		JSONLoaders::LoadLayout(pane->GetDeviceResources(), pane->GetTitleBarLayout(), data["Title"]);
	}
}
void PaneLoader::ParseContent(Pane* pane, json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Content"), "Pane control with name '{}': 'Content' field is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Content"].is_object(), "Pane control with name '{}': 'Content' field must be a json object that represents a Layout. Invalid Pane object: {}", m_name, data.dump(4));
	JSONLoaders::LoadLayout(pane->GetDeviceResources(), pane->GetContentLayout(), data["Content"]);
}
void PaneLoader::ParseOnMouseEnteredTitleBar(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	if (data.contains("OnMouseEnteredTitleBar"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseEnteredTitleBar"].is_string(), "Pane control with name '{}': 'OnMouseEnteredTitleBar' value must be a string. Invalid Pane object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseEnteredTitleBar"].get<std::string>();
		JSON_LOADER_EXCEPTION_IF_FALSE(JSONLoaders::ControlFunctionKeyExists(key), "Pane control with name '{}': 'OnMouseEnteredTitleBar' value ('{}') does not exist in the functions map. Invalid Pane object: {}", m_name, key, data.dump(4));
		pane->SetOnMouseEnteredTitleBarCallback(JSONLoaders::GetControlFunction(key));
	}
}
void PaneLoader::ParseOnMouseExitedTitleBar(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	if (data.contains("OnMouseExitedTitleBar"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseExitedTitleBar"].is_string(), "Pane control with name '{}': 'OnMouseExitedTitleBar' value must be a string. Invalid Pane object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseExitedTitleBar"].get<std::string>();
		JSON_LOADER_EXCEPTION_IF_FALSE(JSONLoaders::ControlFunctionKeyExists(key), "Pane control with name '{}': 'OnMouseExitedTitleBar' value ('{}') does not exist in the functions map. Invalid Pane object: {}", m_name, key, data.dump(4));
		pane->SetOnMouseExitedTitleBarCallback(JSONLoaders::GetControlFunction(key));
	}
}
void PaneLoader::ParseOnMouseEnteredContentRegion(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	if (data.contains("OnMouseEnteredContentRegion"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseEnteredContentRegion"].is_string(), "Pane control with name '{}': 'OnMouseEnteredContentRegion' value must be a string. Invalid Pane object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseEnteredContentRegion"].get<std::string>();
		JSON_LOADER_EXCEPTION_IF_FALSE(JSONLoaders::ControlFunctionKeyExists(key), "Pane control with name '{}': 'OnMouseEnteredContentRegion' value ('{}') does not exist in the functions map. Invalid Pane object: {}", m_name, key, data.dump(4));
		pane->SetOnMouseEnteredContentRegionCallback(JSONLoaders::GetControlFunction(key));
	}
}
void PaneLoader::ParseOnMouseExitedContentRegion(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	if (data.contains("OnMouseExitedContentRegion"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseExitedContentRegion"].is_string(), "Pane control with name '{}': 'OnMouseExitedContentRegion' value must be a string. Invalid Pane object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseExitedContentRegion"].get<std::string>();
		JSON_LOADER_EXCEPTION_IF_FALSE(JSONLoaders::ControlFunctionKeyExists(key), "Pane control with name '{}': 'OnMouseExitedContentRegion' value ('{}') does not exist in the functions map. Invalid Pane object: {}", m_name, key, data.dump(4));
		pane->SetOnMouseExitedContentRegionCallback(JSONLoaders::GetControlFunction(key));
	}
}
void PaneLoader::ParseOnMouseMoved(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	if (data.contains("OnMouseMoved"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseMoved"].is_string(), "Pane control with name '{}': 'OnMouseMoved' value must be a string. Invalid Pane object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseMoved"].get<std::string>();
		JSON_LOADER_EXCEPTION_IF_FALSE(JSONLoaders::ControlFunctionKeyExists(key), "Pane control with name '{}': 'OnMouseMoved' value ('{}') does not exist in the functions map. Invalid Pane object: {}", m_name, key, data.dump(4));
		pane->SetOnMouseMovedCallback(JSONLoaders::GetControlFunction(key));
	}
}

}