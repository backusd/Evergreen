#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/UI/Layout.h"
#include "Evergreen/UI/Controls/Text.h"
#include "Evergreen/UI/GlobalJsonData.h"

#include "Evergreen/Utils/std_format_specializations.h"

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
	virtual ~ControlLoader() noexcept {}

protected:
	ControlLoader() noexcept;
	virtual bool PreLoadValidation(Layout* parent, json& data, const std::string& name) noexcept { return true; }

	template<typename T>
	bool LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, GlobalJsonData* globalData) noexcept;

	std::optional<RowColumnPosition> ParseRowColumnPosition(json& data) noexcept;

	std::unordered_map<std::string, std::function<bool(Control*, json&, Layout*, GlobalJsonData*)>> m_keyLoaders;

};
#pragma warning( pop )


template<typename T>
bool ControlLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& name, GlobalJsonData* globalData) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, std::format("{}:{} - Control with name '{}': deviceResources cannot be nullptr", __FILE__, __LINE__, name));
	EG_CORE_ASSERT(parentLayout != nullptr, std::format("{}:{} - Control with name '{}': parentLayout cannot be nullptr", __FILE__, __LINE__, name));
	EG_CORE_ASSERT(globalData != nullptr, std::format("{}:{} - Control with name '{}': GlobalJsonData cannot be nullptr", __FILE__, __LINE__, name));

	// Load implementation is part of the base class (ControlLoader).
	// It will iterate over the keys the json data and attempt to call the correct
	// parse function that should be stored in the m_keyLoaders map

	// First preform validation which can be customized by an override in the derived class
	if (!PreLoadValidation(parentLayout, data, name))
	{
		EG_CORE_ERROR("{}:{} - Control with name '{}': Call to PreLoadValidation failed.", __FILE__, __LINE__, name);
		return false;
	}

	// Determine Row/Column positioning, create the control, then iterate over the other json keys
	std::optional<RowColumnPosition> positionOpt = ParseRowColumnPosition(data);
	if (!positionOpt.has_value())
	{
		EG_CORE_ERROR("{}:{} - Control with name '{}': Failed to parse row/column positioning.", __FILE__, __LINE__, name);
		return false;
	}

	if (std::optional<T*> control = parentLayout->AddControl<T>(positionOpt.value(), deviceResources))
	{
		// Set the name for the control
		control.value()->Name(name);

		// Iterate over data keys
		constexpr std::array skippableKeys = { "Type", "Row", "Column", "RowSpan", "ColumnSpan" };
		for (auto& [key, value] : data.items())
		{
			if (std::find(skippableKeys.begin(), skippableKeys.end(), key) != skippableKeys.end())
				continue;

			// Use an assert here because if it triggers, it means there is a flaw in the code
			EG_CORE_ASSERT(key.compare("import") != 0, std::format("Control with name '{}': Found 'import' after it should have already been parsed and removed.", name));

			if (m_keyLoaders.find(key) == m_keyLoaders.end())
			{
				EG_CORE_ERROR("{}:{} - Control with name '{}': Failed to parse json - Unrecognized key: {}", __FILE__, __LINE__, name, key);
				return false;
			}

			if (!m_keyLoaders[key](static_cast<Control*>(control.value()), data, parentLayout, globalData))
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

}

