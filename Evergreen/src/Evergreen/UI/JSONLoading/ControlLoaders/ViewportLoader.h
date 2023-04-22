#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/Viewport.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ViewportLoader : public ControlLoader
{
public:
	ViewportLoader(const ViewportLoader&) = delete;
	ViewportLoader& operator=(const ViewportLoader&) = delete;
	~ViewportLoader() noexcept override {}

	template<typename T>
	ND static inline Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) requires (std::is_base_of_v<Viewport, T>)
	{
		return Get().LoadImpl<T>(deviceResources, parent, data, name, rowColumnPositionOverride);
	}

private:
	ViewportLoader() noexcept = default;

	static ViewportLoader& Get() noexcept
	{
		static ViewportLoader loader;
		return loader;
	}

	template<typename T>
	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<Viewport, T>);

	void ParseContent(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);

};
#pragma warning( pop )

template<typename T>
Control* ViewportLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<Viewport, T>)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique

	// Parse Row/Column
	RowColumnPosition rowCol = rowColumnPositionOverride.has_value() ? rowColumnPositionOverride.value() : ParseRowColumnPosition(data);

	// Parse Margin
	Margin margin = ParseMargin(data);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "Margin", "Content" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Viewport control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	T* vp = parent->CreateControl<T>(
		rowCol,
		deviceResources,
		margin
	);
	EG_CORE_ASSERT(vp != nullptr, "Something went wrong, viewport is nullptr");

	vp->Name(name);
	vp->ID(ParseID(data));

	ParseContent(vp->GetDeviceResources(), vp->GetLayout(), data);

	return vp;
}

}