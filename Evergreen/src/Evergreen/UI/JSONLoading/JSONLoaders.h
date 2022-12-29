#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/UI/Layout.h"
#include "Evergreen/UI/Styles/Style.h"
#include "Evergreen/UI/Controls/Control.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/Exceptions/JSONLoadersException.h"

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
	using StyleLoaderFn = std::function<std::unique_ptr<Style>(std::shared_ptr<DeviceResources>, const json&, const std::string&)>;


public:
	JSONLoaders(const JSONLoaders&) = delete;
	void operator=(const JSONLoaders&) = delete;
	~JSONLoaders() noexcept = default;

	static void AddControlLoader(std::string key, ControlLoaderFn loader) noexcept { Get().AddControlLoaderImpl(key, loader); }
	static void AddStyleLoader(std::string key, StyleLoaderFn loader) noexcept { Get().AddStyleLoaderImpl(key, loader); }

	static void LoadLayout(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data) { Get().LoadLayoutDetails(deviceResources, layout, data); }
	static Control* LoadControl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, const json& data, const std::string& name) { return Get().LoadControlImpl(deviceResources, key, parent, data, name); }
	static std::unique_ptr<Style> LoadStyle(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, const json& data, const std::string& stylename) { return std::move(Get().LoadStyleImpl(deviceResources, key, data, stylename)); }
	static std::unique_ptr<ColorBrush> LoadBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data);
	static D2D1_COLOR_F LoadColor(const json& data);

	static bool LoadUI(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept { return Get().LoadUIImpl(deviceResources, rootDirectory, rootFile, rootLayout); }
	static json LoadJSONFile(std::filesystem::path filePath);

	static void ImportJSON(json& data) { Get().ImportJSONImpl(data); }


	static bool IsControlKey(const std::string& controlKey) noexcept { return Get().IsControlKeyImpl(controlKey); }
	static bool IsStyleKey(const std::string& styleKey) noexcept { return Get().IsStyleKeyImpl(styleKey); }

	static void ClearCache() noexcept { Get().ClearCacheImpl(); }

	static void AddControlFunction(const std::string& key, std::function<void(Control*)> func) noexcept { Get().AddControlFunctionImpl(key, func); }
	static bool ControlFunctionKeyExists(const std::string& key) noexcept { return Get().ControlFunctionKeyExistsImpl(key); }
	static std::function<void(Control*)> GetControlFunction(const std::string& key) { return Get().GetControlFunctionImpl(key); }


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

	Control* LoadControlImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, const json& data, const std::string& name);
	std::unique_ptr<Style> LoadStyleImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, const json& data, const std::string& stylename);

	static std::unique_ptr<ColorBrush> LoadSolidColorBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data);
	static std::unique_ptr<ColorBrush> LoadGradientBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data);
	static std::unique_ptr<ColorBrush> LoadRadialBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data);
	static std::unique_ptr<ColorBrush> LoadBitmapBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data);

	
	bool LoadUIImpl(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept;
	void LoadGlobalStyles(std::shared_ptr<DeviceResources> deviceResources);
	void LoadLayoutDetails(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);
	

	void LoadLayoutBrush(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);
	void LoadLayoutRowDefinitions(Layout* layout, json& data);
	void LoadLayoutColumnDefinitions(Layout* layout, json& data);
	void LoadSubLayout(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name);

	RowColumnPosition ParseRowColumnPosition(json& data);
	std::tuple<RowColumnType, float> ParseRowColumnTypeAndSize(json& data, Layout* layout);

	void ImportJSONImpl(json& data);


	bool IsControlKeyImpl(const std::string& controlKey) const noexcept { return m_controlLoaders.find(controlKey) != m_controlLoaders.end(); }
	bool IsStyleKeyImpl(const std::string& styleKey) const noexcept { return m_styleLoaders.find(styleKey) != m_styleLoaders.end(); }

	void AddControlFunctionImpl(const std::string& key, std::function<void(Control*)> func) noexcept { m_functionsMap[key] = func; }
	bool ControlFunctionKeyExistsImpl(const std::string& key) noexcept { return m_functionsMap.contains(key); }
	std::function<void(Control*)> GetControlFunctionImpl(const std::string& key) { return m_functionsMap[key]; }


	std::unordered_map<std::string, ControlLoaderFn>	m_controlLoaders; 
	std::unordered_map<std::string, StyleLoaderFn>		m_styleLoaders;

	// Keep a cache of styles that have been parsed for quick lookup
	std::unordered_map<std::string, std::unique_ptr<Style>> m_stylesCache;

	// Map of function callbacks
	std::unordered_map<std::string, std::function<void(Control*)>> m_functionsMap;

	json					m_jsonRoot;
	std::filesystem::path	m_jsonRootDirectory;
};
#pragma warning( pop )
}