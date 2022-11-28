#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/UI/Layout.h"
#include "Evergreen/UI/Controls/Text.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ControlLoader
{
public:
	ControlLoader(const ControlLoader&) = delete;
	void operator=(const ControlLoader&) = delete;
	virtual ~ControlLoader() {}

protected:
	ControlLoader() noexcept;

	template<typename T>
	bool LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name) noexcept;

	bool ParseRowColumnPosition(json& data, RowColumnPosition& position) noexcept;

	std::unordered_map<std::string, std::function<bool(Control*, json&, Layout*)>> m_keyLoaders;

};
#pragma warning( pop )


template<typename T>
bool ControlLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& name) noexcept
{
	// Load implementation is part of the base class (ControlLoader).
	// It will iterate over the keys the json data and attempt to call the correct
	// parse function that should be stored in the m_keyLoaders map

	// Determine Row/Column positioning, create the control, then iterate over the other json keys
	RowColumnPosition position;
	if (!ParseRowColumnPosition(data, position))
	{
		EG_CORE_ERROR("{}:{} - Control with name '{}': Failed to parse row/column positioning.", __FILE__, __LINE__, name);
		return false;
	}

	if (std::optional<T*> control = parentLayout->AddControl<T>(position, deviceResources))
	{
		// Set the name for the control
		control.value()->Name(name);

		// Iterate over data keys
		constexpr std::array skippableKeys = { "Type", "Row", "Column", "RowSpan", "ColumnSpan" };
		for (auto& [key, value] : data.items())
		{
			if (std::find(skippableKeys.begin(), skippableKeys.end(), key) != skippableKeys.end())
				continue;

			if (key.compare("import") == 0)
			{
				EG_CORE_ERROR("{}:{} - Control with name '{}': Found 'import' after it should have already been parsed and removed.", __FILE__, __LINE__, name);
				return false;
			}

			if (m_keyLoaders.find(key) == m_keyLoaders.end())
			{
				EG_CORE_ERROR("{}:{} - Control with name '{}': Failed to parse json - Unrecognized key: {}", __FILE__, __LINE__, name, key);
				return false;
			}

			if (!m_keyLoaders[key](static_cast<Control*>(control.value()), data, parentLayout))
			{
				EG_CORE_ERROR("{}:{} - Control with name '{}': Key Loader function failed for key: {}", __FILE__, __LINE__, name, key);
				return false;
			}
		}

		return true;
	}

	EG_CORE_ERROR("{}:{} - Text control with name '{}': Failed to add control to the layout.", __FILE__, __LINE__, name);
	return false;
}


// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API TextLoader : public ControlLoader
{
public:
	TextLoader(const TextLoader&) = delete;
	void operator=(const TextLoader&) = delete;
	virtual ~TextLoader() {}

	static bool Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name) noexcept { return Get().LoadImpl<Text>(deviceResources, parent, data, name); }

private:
	TextLoader() noexcept;

	static TextLoader& Get() noexcept
	{
		static TextLoader loader;
		return loader;
	}

	bool ParseText(Text* textControl, json& data, Layout* parentLayout) noexcept;



	... Create all necessary Parse functions to be able to parse Text control






};
#pragma warning( pop )


}

template <>
struct std::formatter<json> : std::formatter<std::string> {
	auto format(json& _json, std::format_context& ctx) {
		std::ostringstream oss;
		oss << _json;
		return formatter<std::string>::format(oss.str(), ctx);
	}
};