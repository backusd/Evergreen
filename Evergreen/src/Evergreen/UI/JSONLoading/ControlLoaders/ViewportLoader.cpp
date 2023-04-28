#include "pch.h"
#include "ViewportLoader.h"

namespace Evergreen
{

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

	std::string type = content["Type"].get<std::string>();
	JSON_LOADER_EXCEPTION_IF_FALSE(JSONLoaders::IsLayoutKey(type), "Viewport control with name '{}': The 'Content'->'Type' value '{}' was not found in the map of Layout loaders. Invalid Viewport object: {}", m_name, type, data.dump(4));
	
	JSONLoaders::LoadLayout(deviceResources, type, layout, content, std::format("{}_ContentLayout", m_name));
}

}