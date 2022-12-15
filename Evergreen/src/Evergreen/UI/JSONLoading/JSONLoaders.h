#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/UI/Layout.h"
#include "Evergreen/UI/Styles/Style.h"
#include "Evergreen/UI/Controls/Control.h"
#include "Evergreen/UI/Brushes/Brushes.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API JSONLoaders
{
	using ControlLoaderFn = std::function<Control*(std::shared_ptr<DeviceResources>, Layout*, const json&, const std::string&)>;
	using StyleLoaderFn = std::function<std::shared_ptr<Style>(std::shared_ptr<DeviceResources>, const json&, const std::string&)>;


public:
	JSONLoaders(const JSONLoaders&) = delete;
	void operator=(const JSONLoaders&) = delete;
	~JSONLoaders() noexcept = default;

	static void AddControlLoader(std::string key, ControlLoaderFn loader) noexcept { Get().AddControlLoaderImpl(key, loader); }
	static void AddStyleLoader(std::string key, StyleLoaderFn loader) noexcept { Get().AddStyleLoaderImpl(key, loader); }

	static Control* LoadControl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, const json& data, const std::string& name) noexcept { return Get().LoadControlImpl(deviceResources, key, parent, data, name); }
	static std::shared_ptr<Style> LoadStyle(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, const json& data, const std::string& stylename) noexcept { return Get().LoadStyleImpl(deviceResources, key, data, stylename); }
	static std::unique_ptr<ColorBrush> LoadBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept;
	static std::optional<D2D1_COLOR_F> LoadColor(const json& data) noexcept;

	static bool IsControlKey(const std::string& controlKey) noexcept { return Get().IsControlKeyImpl(controlKey); }
	static bool IsStyleKey(const std::string& styleKey) noexcept { return Get().IsStyleKeyImpl(styleKey); }

	static void ClearCache() noexcept { Get().ClearCacheImpl(); }

private:
	JSONLoaders() noexcept = default;

	static JSONLoaders& Get() noexcept
	{
		static JSONLoaders singleton;
		return singleton;
	}

	void ClearCacheImpl() noexcept { m_stylesCache.clear(); }

	void AddControlLoaderImpl(std::string key, ControlLoaderFn loader) noexcept { m_controlLoaders[key] = loader; }
	void AddStyleLoaderImpl(std::string key, StyleLoaderFn loader) noexcept { m_styleLoaders[key] = loader; }

	Control* LoadControlImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, const json& data, const std::string& name) noexcept;
	std::shared_ptr<Style> LoadStyleImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, const json& data, const std::string& stylename) noexcept;

	static std::unique_ptr<ColorBrush> LoadSolidColorBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept;
	static std::unique_ptr<ColorBrush> LoadGradientBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept;
	static std::unique_ptr<ColorBrush> LoadRadialBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept;
	static std::unique_ptr<ColorBrush> LoadBitmapBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept;

	bool IsControlKeyImpl(const std::string& controlKey) const noexcept { return m_controlLoaders.find(controlKey) != m_controlLoaders.end(); }
	bool IsStyleKeyImpl(const std::string& styleKey) const noexcept { return m_styleLoaders.find(styleKey) != m_styleLoaders.end(); }

	std::unordered_map<std::string, ControlLoaderFn>	m_controlLoaders; 
	std::unordered_map<std::string, StyleLoaderFn>		m_styleLoaders;

	// Keep a cache of styles that have been parsed for quick lookup
	std::unordered_map<std::string, std::shared_ptr<Style>> m_stylesCache;
};
#pragma warning( pop )
}