#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/Rendering/DeviceResources.h"
#include "Evergreen/UI/Colors/Brushes.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

/*
namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API BrushLoader
{
public:
	BrushLoader(const BrushLoader&) = delete;
	void operator=(const BrushLoader&) = delete;

protected:
	BrushLoader() noexcept;
	virtual bool PreLoadValidation(json& data) noexcept { return true; }

	//std::optional<std::unique_ptr<ColorBrush>> LoadImpl2(std::shared_ptr<DeviceResources> deviceResources, json& data) noexcept;


	template<typename T>
	std::optional<std::unique_ptr<ColorBrush>> LoadImpl(std::shared_ptr<DeviceResources> deviceResources, json& data) noexcept;

	std::unordered_map<std::string, std::function<bool(ColorBrush*, json&)>> m_keyLoaders;
};
#pragma warning( pop )

template<typename T>
std::optional<std::unique_ptr<ColorBrush>> BrushLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, std::format("{}:{} - BrushLoader: deviceResources cannot be nullptr", __FILE__, __LINE__));

	// Load implementation needs to be part of the base class (BrushLoader).
	// It will iterate over the keys the json data and attempt to call the correct
	// parse function that should be stored in the m_keyLoaders map

	// First preform validation which can be customized by an override in the derived class
	if (!PreLoadValidation(data))
	{
		EG_CORE_ERROR("{}:{} - BrushLoader: Call to PreLoadValidation failed.", __FILE__, __LINE__);
		return std::nullopt;
	}

	// Create unique_ptr to the colorbrush, then call keyLoader functions to set the brush attributes
	std::unique_ptr<T> brush = std::make_unique<T>(deviceResources);
	if (brush == nullptr)
	{
		EG_CORE_ERROR("{}:{} - Something went wrong. Creating the color brush returned nullptr.", __FILE__, __LINE__);
		return std::nullopt;
	}

	for (auto& [key, value] : data.items())
	{
		// Use an assert here because if it triggers, it means there is a flaw in the code
		EG_CORE_ASSERT(key.compare("import") != 0, "BrushLoader: Found 'import' after it should have already been parsed and removed.");

		if (m_keyLoaders.find(key) == m_keyLoaders.end())
		{
			EG_CORE_TRACE("{}:{} - BrushLoader: Skipping unrecognized key: {}", __FILE__, __LINE__, key);
			continue;
		}

		if (!m_keyLoaders[key](static_cast<ColorBrush*>(brush.get()), data))
		{
			EG_CORE_ERROR("{}:{} - BrushLoader: Key Loader function failed for key: {}", __FILE__, __LINE__, key);
			return std::nullopt;
		}
	}

	return std::move(brush);
}
}
*/