#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/UI/Layout.h"
#include "Evergreen/UI/Styles/Style.h"
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
class EVERGREEN_API StyleLoader
{
public:
	StyleLoader(const StyleLoader&) = delete;
	void operator=(const StyleLoader&) = delete;
	virtual ~StyleLoader() noexcept {}

protected:
	StyleLoader() noexcept;
	virtual bool PreLoadValidation(json& data, const std::string& name) noexcept { return true; }

	template<typename T>
	std::optional<std::shared_ptr<Style>> LoadImpl(std::shared_ptr<DeviceResources> deviceResources, json& data, const std::string& name) noexcept;

	std::unordered_map<std::string, std::function<bool(Style*, json&)>> m_keyLoaders;
};
#pragma warning( pop )

template<typename T>
std::optional<std::shared_ptr<Style>> StyleLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, json& data, const std::string& name) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, std::format("{}:{} - Style with name '{}': deviceResources cannot be nullptr", __FILE__, __LINE__, name));

	// Load implementation needs to be part of the base class (StyleLoader).
	// It will iterate over the keys the json data and attempt to call the correct
	// parse function that should be stored in the m_keyLoaders map

	// First preform validation which can be customized by an override in the derived class
	if (!PreLoadValidation(data, name))
	{
		EG_CORE_ERROR("{}:{} - Style with name '{}': Call to PreLoadValidation failed.", __FILE__, __LINE__, name);
		return std::nullopt;
	}

	// Create shared_ptr to the style, then call keyLoader functions to set the style attributes
	std::shared_ptr<T> style = std::make_shared<T>(deviceResources, name);
	if (style == nullptr)
	{
		EG_CORE_ERROR("{}:{} - Something went wrong. Creating the style with name '{}' returned nullptr.", __FILE__, __LINE__, name);
		return std::nullopt;
	}

	for (auto& [key, value] : data.items())
	{
		// Use an assert here because if it triggers, it means there is a flaw in the code
		EG_CORE_ASSERT(key.compare("import") != 0, std::format("Style with name '{}': Found 'import' after it should have already been parsed and removed.", name));

		if (m_keyLoaders.find(key) == m_keyLoaders.end())
		{
			EG_CORE_TRACE("{}:{} - Style with name '{}': Skipping unrecognized key: {}", __FILE__, __LINE__, name, key);
			continue;
		}

		if (!m_keyLoaders[key](static_cast<Style*>(style.get()), data))
		{
			EG_CORE_ERROR("{}:{} - Style with name '{}': Key Loader function failed for key: {}", __FILE__, __LINE__, name, key);
			return std::nullopt;
		}
	}

	return std::static_pointer_cast<Style>(style);
}

}