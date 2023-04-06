#include "pch.h"
#include "ViewportLoader.h"

namespace Evergreen
{
Control* ViewportLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique

	// Parse Row/Column
	RowColumnPosition rowCol = rowColumnPositionOverride.has_value() ? rowColumnPositionOverride.value() : ParseRowColumnPosition(data);

	// Parse Margin
	Margin margin = ParseMargin(data);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"Content", "OnChar", "OnKeyPressed", "OnKeyReleased", "OnMouseEntered", "OnMouseExited", "OnMouseMoved", 
	"OnMouseScrolledVertical", "OnMouseScrolledHorizontal", "OnMouseButtonPressed", "OnMouseButtonReleased", 
	"OnClick", "OnDoubleClick", "OnUpdate"};
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Viewport control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	Viewport* vp = parent->CreateControl<Viewport>(
		rowCol,
		deviceResources,
		margin
		);
	EG_CORE_ASSERT(vp != nullptr, "Something went wrong, viewport is nullptr");

	vp->Name(name);
	vp->ID(ParseID(data));

	ParseContent(vp->GetDeviceResources(), vp->GetLayout(), data);

	ParseOnChar(vp, data);
	ParseOnKeyPressed(vp, data);
	ParseOnKeyReleased(vp, data);
	ParseOnMouseEntered(vp, data);
	ParseOnMouseExited(vp, data);
	ParseOnMouseMoved(vp, data);
	ParseOnMouseButtonPressed(vp, data);
	ParseOnMouseButtonReleased(vp, data);
	ParseOnMouseScrolledVertical(vp, data);
	ParseOnMouseScrolledHorizontal(vp, data);
	ParseOnClick(vp, data);
	ParseOnDoubleClick(vp, data);

	ParseOnUpdateCallback(vp, data);

	return vp;
}

void ViewportLoader::ParseContent(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "layout is nullptr");

	// If no content is given, just leave the layout empty, but in order for the layout to be valid, there must be at least one row and column
	if (!data.contains("Content"))
	{
		layout->AddRow({ RowColumnType::STAR, 1.0f });
		layout->AddColumn({ RowColumnType::STAR, 1.0f });
		return;
	}

	JSON_LOADER_EXCEPTION_IF_FALSE(data["Content"].is_object(), "Viewport control with name '{}': 'Content' value must be a json object. Invalid Viewport object: {}", m_name, data.dump(4));

	json content = data["Content"];

	JSON_LOADER_EXCEPTION_IF_FALSE(content.contains("Type"), "Viewport control with name '{}': 'Content' json object must contain key 'Type'. Invalid Viewport object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(content["Type"].is_string(), "Viewport control with name '{}': 'Content' json object key 'Type' must have a string value. Invalid Viewport object: {}", m_name, data.dump(4));

	std::string contentType = content["Type"].get<std::string>();
	JSON_LOADER_EXCEPTION_IF_FALSE(contentType.compare("Layout") == 0, "Viewport control with name '{}': 'Content'->'Type' must have the value 'Layout'. Invalid Viewport object: {}", m_name, data.dump(4));

	JSONLoaders::LoadLayout(deviceResources, layout, content);
}

void ViewportLoader::ParseOnChar(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnChar"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnChar"].is_string(), "Viewport control with name '{}': 'OnChar' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnChar"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, CharEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnChar' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnCharCallback(callback);
	}
}
void ViewportLoader::ParseOnKeyPressed(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnKeyPressed"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnKeyPressed"].is_string(), "Viewport control with name '{}': 'OnKeyPressed' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnKeyPressed"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, KeyPressedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnKeyPressed' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnKeyPressedCallback(callback);
	}
}
void ViewportLoader::ParseOnKeyReleased(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnKeyReleased"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnKeyReleased"].is_string(), "Viewport control with name '{}': 'OnKeyReleased' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnKeyReleased"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, KeyReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnKeyReleased' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnKeyReleasedCallback(callback);
	}
}
void ViewportLoader::ParseOnMouseEntered(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnMouseEntered"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseEntered"].is_string(), "Viewport control with name '{}': 'OnMouseEntered' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseEntered"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnMouseEntered' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnMouseEnteredCallback(callback);
	}
}
void ViewportLoader::ParseOnMouseExited(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnMouseExited"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseExited"].is_string(), "Viewport control with name '{}': 'OnMouseExited' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseExited"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnMouseExited' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnMouseExitedCallback(callback);
	}
}
void ViewportLoader::ParseOnMouseMoved(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnMouseMoved"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseMoved"].is_string(), "Viewport control with name '{}': 'OnMouseMoved' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseMoved"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnMouseMoved' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnMouseMovedCallback(callback);
	}
}
void ViewportLoader::ParseOnMouseButtonPressed(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnMouseButtonPressed"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseButtonPressed"].is_string(), "Viewport control with name '{}': 'OnMouseButtonPressed' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseButtonPressed"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseButtonPressedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnMouseButtonPressed' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnMouseButtonPressedCallback(callback);
	}
}
void ViewportLoader::ParseOnMouseButtonReleased(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnMouseButtonReleased"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseButtonReleased"].is_string(), "Viewport control with name '{}': 'OnMouseButtonReleased' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseButtonReleased"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnMouseButtonReleased' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnMouseButtonReleasedCallback(callback);
	}
}
void ViewportLoader::ParseOnMouseScrolledVertical(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnChar"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseScrolledVertical"].is_string(), "Viewport control with name '{}': 'OnMouseScrolledVertical' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseScrolledVertical"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseScrolledEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnMouseScrolledVertical' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnMouseScrolledVerticalCallback(callback);
	}
}
void ViewportLoader::ParseOnMouseScrolledHorizontal(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnMouseScrolledHorizontal"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseScrolledHorizontal"].is_string(), "Viewport control with name '{}': 'OnMouseScrolledHorizontal' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseScrolledHorizontal"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseScrolledEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnMouseScrolledHorizontal' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnMouseScrolledHorizontalCallback(callback);
	}
}
void ViewportLoader::ParseOnClick(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnChar"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnClick"].is_string(), "Viewport control with name '{}': 'OnClick' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnClick"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnClick' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnClickCallback(callback);
	}
}
void ViewportLoader::ParseOnDoubleClick(Viewport* vp, json& data)
{
	EG_CORE_ASSERT(vp != nullptr, "viewport is nullptr");

	if (data.contains("OnDoubleClick"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnDoubleClick"].is_string(), "Viewport control with name '{}': 'OnDoubleClick' value must be a string. Invalid Viewport object: {}", m_name, data.dump(4));
		std::string key = data["OnDoubleClick"].get<std::string>();
		auto callback = JSONLoaders::GetCallback<Viewport, MouseButtonDoubleClickEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Viewport control with name '{}': 'OnDoubleClick' callback not found for key '{}'. Invalid Viewport object: {}", m_name, key, data.dump(4));
		vp->SetOnDoubleClickCallback(callback);
	}
}

}