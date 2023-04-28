#pragma once
#include "pch.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Layout.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API LayoutLoader
{
public:
	LayoutLoader(const LayoutLoader&) = delete;
	LayoutLoader& operator=(const LayoutLoader&) = delete;
	~LayoutLoader() noexcept {}

	template<typename T>
	ND static inline void Load(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data, const std::string& name) requires (std::is_base_of_v<Layout, T>)
	{
		Get().LoadImpl<T>(deviceResources, layout, data, name);
	}

private:
	LayoutLoader() noexcept = default;

	static LayoutLoader& Get() noexcept
	{
		static LayoutLoader loader;
		return loader;
	}

	template<typename T>
	void LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data, const std::string& name) requires (std::is_base_of_v<Layout, T>);

	std::tuple<RowColumnType, float> ParseRowColumnTypeAndSize(json& data, const std::string& layoutName);
	RowColumnPosition ParseRowColumnPosition(json& data);

	void LoadLayoutBrush(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);
	void LoadLayoutRowDefinitions(Layout* layout, json& data);
	void LoadLayoutColumnDefinitions(Layout* layout, json& data);
	void LoadLayoutMargin(Layout* layout, json& data);
	void LoadLayoutBorder(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);
	void LoadLayoutID(Layout* layout, json& data);



};
#pragma warning( pop )

template<typename T>
void LayoutLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data, const std::string& name) requires (std::is_base_of_v<Layout, T>)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// First, import any necessary data
	JSONLoaders::ImportJSON(data); 

	// Load layout brush
	LoadLayoutBrush(deviceResources, layout, data); 

	// Load Row/Column definitions
	LoadLayoutRowDefinitions(layout, data); 
	LoadLayoutColumnDefinitions(layout, data); 

	// Load Margin
	LoadLayoutMargin(layout, data); 

	// Load Layout Border details
	LoadLayoutBorder(deviceResources, layout, data); 

	// Now iterate over the controls and sublayouts within the layout
	for (auto& [key, value] : data.items()) 
	{
		if (key.compare("import") == 0 ||
			key.compare("id") == 0 ||
			key.compare("Type") == 0 ||
			key.compare("Brush") == 0 ||
			key.compare("Row") == 0 ||
			key.compare("Column") == 0 ||
			key.compare("RowSpan") == 0 ||
			key.compare("ColumnSpan") == 0 ||
			key.compare("RowDefinitions") == 0 ||
			key.compare("ColumnDefinitions") == 0 ||
			key.compare("Margin") == 0 ||
			key.compare("BorderBrush") == 0 ||
			key.compare("BorderWidth") == 0 ||
			key.compare("BorderTopLeftOffsetX") == 0 ||
			key.compare("BorderTopLeftOffsetY") == 0 ||
			key.compare("BorderTopRightOffsetX") == 0 ||
			key.compare("BorderTopRightOffsetY") == 0 ||
			key.compare("BorderBottomLeftOffsetX") == 0 ||
			key.compare("BorderBottomLeftOffsetY") == 0 ||
			key.compare("BorderBottomRightOffsetX") == 0 ||
			key.compare("BorderBottomRightOffsetY") == 0 ||
			key.compare("OnResize") == 0)
			continue;

		JSON_LOADER_EXCEPTION_IF_FALSE(data[key].contains("Type"), "Control or sub-layout has no 'Type' definition: {}", data[key].dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data[key]["Type"].is_string(), "Control or sub-layout 'Type' definition must be a string.\nInvalid value : {}", data[key]["Type"].dump(4));

		std::string type = data[key]["Type"].get<std::string>();

		// Load either a sublayout or control
		if (type.compare("Layout") == 0 || JSONLoaders::IsLayoutKey(type))
		{
			// First, import any necessary data
			JSONLoaders::ImportJSON(data);

			// Parse the row/column/rowspan/columnspan values
			RowColumnPosition position = ParseRowColumnPosition(data);

			Layout* sublayout = layout->AddSubLayout(position, name); 
			JSONLoaders::LoadLayout(deviceResources, type, sublayout, data[key], key);
		}
		else if (JSONLoaders::IsControlKey(type))
		{
			JSONLoaders::ImportJSON(data[key]); 

			Control* control = JSONLoaders::LoadControl(deviceResources, type, layout, data[key], key);
			if (control == nullptr)
				EG_CORE_ERROR("Failed to load control with name '{}'.", key);
		}
		else
		{
			EG_CORE_WARN("Attempting to load control: {} (... not yet supported ...)", type);
		}
	}
}


}