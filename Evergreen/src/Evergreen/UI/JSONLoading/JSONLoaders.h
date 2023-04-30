#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/UI/Layout.h"
#include "Evergreen/UI/Styles/Style.h"
#include "Evergreen/UI/Controls.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/Exceptions/JSONLoadersException.h"
#include "Evergreen/Events/Event.h"

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
	using ControlLoaderFn = std::function<Control*(std::shared_ptr<DeviceResources>, Layout*, json&, const std::string&, std::optional<RowColumnPosition>)>;
	using StyleLoaderFn = std::function<std::unique_ptr<Style>(std::shared_ptr<DeviceResources>, json&, const std::string&)>;
	using LayoutLoaderFn = std::function<void(std::shared_ptr<DeviceResources>, Layout*, json&, const std::string&)>;

public:
	JSONLoaders(const JSONLoaders&) = delete;
	JSONLoaders& operator=(const JSONLoaders&) = delete;
	~JSONLoaders() noexcept = default;

	static void AddControlLoader(std::string key, ControlLoaderFn loader) noexcept { Get().AddControlLoaderImpl(key, loader); }
	static void AddStyleLoader(std::string key, StyleLoaderFn loader) noexcept { Get().AddStyleLoaderImpl(key, loader); }
	static void AddLayoutLoader(std::string key, LayoutLoaderFn loader) noexcept { Get().AddLayoutLoaderImpl(key, loader); }

	static void LoadControlsFromFile(const std::string& fileName, Layout* parentLayout, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { Get().LoadControlsFromFileImpl(fileName, parentLayout, rowColumnPositionOverride); }
	static void LoadLayoutFromFile(const std::string& fileName, Layout* layoutToFill) { Get().LoadLayoutFromFileImpl(fileName, layoutToFill); }

	static inline void LoadLayout(std::shared_ptr<DeviceResources> deviceResources, const std::string& type, Layout* layout, json& data, const std::string& name) { Get().LoadLayoutImpl(deviceResources, type, layout, data, name); }
	
	static Control* LoadControl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadControlImpl(deviceResources, key, parent, data, name, rowColumnPositionOverride); }
	static std::unique_ptr<Style> LoadStyle(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, json& data, const std::string& stylename) { return std::move(Get().LoadStyleImpl(deviceResources, key, data, stylename)); }
	static std::unique_ptr<ColorBrush> LoadBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	static D2D1_COLOR_F LoadColor(json& data);

	static bool LoadUI(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept { return Get().LoadUIImpl(deviceResources, rootDirectory, rootFile, rootLayout); }
	static json LoadJSONFile(std::filesystem::path filePath);

	static void ImportJSON(json& data) { Get().ImportJSONImpl(data); }


	ND static inline bool IsControlKey(const std::string& key) noexcept { return Get().IsControlKeyImpl(key); }
	ND static inline bool IsStyleKey(const std::string& key) noexcept { return Get().IsStyleKeyImpl(key); }
	ND static inline bool IsLayoutKey(const std::string& key) noexcept { return Get().IsLayoutKeyImpl(key); }

	static void AddControlName(const std::string& name) { Get().AddControlNameImpl(name); }
	static void ClearCache() noexcept { Get().ClearCacheImpl(); }



private:
	JSONLoaders() noexcept = default;

	static JSONLoaders& Get() noexcept
	{
		static JSONLoaders singleton;
		return singleton;
	}

	inline void ClearCacheImpl() noexcept { m_stylesCache.clear(); }

	void AddControlLoaderImpl(std::string key, ControlLoaderFn loader) noexcept { m_controlLoaders[key] = loader; }
	void AddStyleLoaderImpl(std::string key, StyleLoaderFn loader) noexcept { m_styleLoaders[key] = loader; }
	void AddLayoutLoaderImpl(std::string key, LayoutLoaderFn loader) noexcept { m_layoutLoaders[key] = loader; }

	Control* LoadControlImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);
	std::unique_ptr<Style> LoadStyleImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, json& data, const std::string& stylename);
	void LoadLayoutImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& type, Layout* layout, json& data, const std::string& name);

	static std::unique_ptr<ColorBrush> LoadSolidColorBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	static std::unique_ptr<ColorBrush> LoadGradientBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	static std::unique_ptr<ColorBrush> LoadRadialBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	static std::unique_ptr<ColorBrush> LoadBitmapBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);

	void LoadControlsFromFileImpl(const std::string& fileName, Layout* parentLayout, std::optional<RowColumnPosition> rowColumnPositionOverride);
	void LoadLayoutFromFileImpl(const std::string& fileName, Layout* layoutToFill);

	bool LoadUIImpl(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept;
	void LoadGlobalStyles(std::shared_ptr<DeviceResources> deviceResources);
	void LoadPanes(std::shared_ptr<DeviceResources> deviceResources, Layout* layout);

	void ImportJSONImpl(json& data);

	ND inline bool IsControlKeyImpl(const std::string& key) const noexcept { return m_controlLoaders.find(key) != m_controlLoaders.end(); }
	ND inline bool IsStyleKeyImpl(const std::string& key) const noexcept { return m_styleLoaders.find(key) != m_styleLoaders.end(); }
	ND inline bool IsLayoutKeyImpl(const std::string& key) const noexcept { return m_layoutLoaders.find(key) != m_layoutLoaders.end(); }

	void AddControlNameImpl(const std::string& name);

	std::unordered_map<std::string, ControlLoaderFn>	m_controlLoaders; 
	std::unordered_map<std::string, StyleLoaderFn>		m_styleLoaders;
	std::unordered_map<std::string, LayoutLoaderFn>		m_layoutLoaders;

	// Keep a cache of styles that have been parsed for quick lookup
	std::unordered_map<std::string, std::unique_ptr<Style>> m_stylesCache;

	// Keep a list of all control names so that we can ensure control names are unique
	std::vector<std::string> m_controlNames;

	json					m_jsonRoot;
	std::filesystem::path	m_jsonRootDirectory;
};
#pragma warning( pop )

}