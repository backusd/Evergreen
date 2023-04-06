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

#define CONTROL_EVENT_MAP(control, evnt) public:                                                                                               \
                                         static void AddCallback(const std::string& key, std::function<void(control*, evnt&)> func) noexcept   \
                                         {                                                                                                     \
                                             Get().AddCallbackImpl(key, func);                                                                 \
                                         }                                                                                                     \
                                         private:                                                                                              \
                                         void AddCallbackImpl(const std::string& key, std::function<void(control*, evnt&)> func) noexcept      \
                                         {                                                                                                     \
                                         	m_map##control##evnt[key] = func;                                                                  \
                                         }                                                                                                     \
                                         template<>                                                                                            \
                                         std::function<void(control*, evnt&)> GetCallbackImpl(const std::string& key)                          \
                                         {                                                                                                     \
                                         	return m_map##control##evnt[key];                                                                  \
                                         }                                                                                                     \
                                         std::unordered_map<std::string, std::function<void(control*, evnt&)>> m_map##control##evnt

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
	using LayoutCallbackFn = std::function<void(Layout*)>;

public:
	JSONLoaders(const JSONLoaders&) = delete;
	void operator=(const JSONLoaders&) = delete;
	~JSONLoaders() noexcept = default;

	static void AddControlLoader(std::string key, ControlLoaderFn loader) noexcept { Get().AddControlLoaderImpl(key, loader); }
	static void AddStyleLoader(std::string key, StyleLoaderFn loader) noexcept { Get().AddStyleLoaderImpl(key, loader); }

	static void LoadControlsFromFile(const std::string& fileName, Layout* parentLayout, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { Get().LoadControlsFromFileImpl(fileName, parentLayout, rowColumnPositionOverride); }
	static void LoadLayout(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data) { Get().LoadLayoutDetails(deviceResources, layout, data); }
	static Control* LoadControl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadControlImpl(deviceResources, key, parent, data, name, rowColumnPositionOverride); }
	static std::unique_ptr<Style> LoadStyle(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, json& data, const std::string& stylename) { return std::move(Get().LoadStyleImpl(deviceResources, key, data, stylename)); }
	static std::unique_ptr<ColorBrush> LoadBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	static D2D1_COLOR_F LoadColor(json& data);

	static bool LoadUI(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept { return Get().LoadUIImpl(deviceResources, rootDirectory, rootFile, rootLayout); }
	static json LoadJSONFile(std::filesystem::path filePath);

	static void ImportJSON(json& data) { Get().ImportJSONImpl(data); }


	static bool IsControlKey(const std::string& controlKey) noexcept { return Get().IsControlKeyImpl(controlKey); }
	static bool IsStyleKey(const std::string& styleKey) noexcept { return Get().IsStyleKeyImpl(styleKey); }

	static void AddControlName(const std::string& name) { Get().AddControlNameImpl(name); }
	static void ClearCache() noexcept { Get().ClearCacheImpl(); }

	static std::tuple<RowColumnType, float> ParseRowColumnTypeAndSize(json& data, const std::string& layoutName) { return Get().ParseRowColumnTypeAndSizeImpl(data, layoutName); }

	static std::function<void(Control*, const Timer&)> GetOnUpdateCallback(const std::string& key) { return Get().GetOnUpdateCallbackImpl(key); }
	static void AddOnUpdateCallback(const std::string& key, std::function<void(Control*, const Timer&)> fn) { Get().AddOnUpdateCallbackImpl(key, fn); }

	static void AddLayoutCallback(const std::string& key, LayoutCallbackFn fn) { Get().AddLayoutCallbackImpl(key, fn); }
	static LayoutCallbackFn GetLayoutCallback(const std::string& key) { return Get().GetLayoutCallbackImpl(key); }

public:
	template <class C, class E>
	static std::function<void(C*, E&)> GetCallback(const std::string& key) { return Get().GetCallbackImpl<C, E>(key); }

private:
	template<class C, class E>
	std::function<void(C*, E&)> GetCallbackImpl(const std::string& key)
	{
		EG_CORE_ERROR("No functions map for Control and Event combination. Key: {}", key);
		std::terminate();
	}

	CONTROL_EVENT_MAP(Button, MouseMoveEvent); 
	CONTROL_EVENT_MAP(Button, MouseButtonPressedEvent);
	CONTROL_EVENT_MAP(Button, MouseButtonReleasedEvent);
	CONTROL_EVENT_MAP(Pane, MouseMoveEvent);
	CONTROL_EVENT_MAP(SliderFloat, MouseMoveEvent);
	CONTROL_EVENT_MAP(SliderFloat, MouseButtonPressedEvent);
	CONTROL_EVENT_MAP(SliderFloat, MouseButtonReleasedEvent);
	CONTROL_EVENT_MAP(SliderFloat, SliderFloatValueChangedEvent);
	CONTROL_EVENT_MAP(SliderInt, MouseMoveEvent);
	CONTROL_EVENT_MAP(SliderInt, MouseButtonPressedEvent);
	CONTROL_EVENT_MAP(SliderInt, MouseButtonReleasedEvent);
	CONTROL_EVENT_MAP(SliderInt, SliderIntValueChangedEvent);
	CONTROL_EVENT_MAP(TextInput, MouseMoveEvent);
	CONTROL_EVENT_MAP(TextInput, MouseButtonPressedEvent);
	CONTROL_EVENT_MAP(TextInput, MouseButtonReleasedEvent);
	CONTROL_EVENT_MAP(TextInput, CharEvent);
	CONTROL_EVENT_MAP(RadioButton, MouseMoveEvent);
	CONTROL_EVENT_MAP(RadioButton, MouseButtonPressedEvent);
	CONTROL_EVENT_MAP(RadioButton, MouseButtonReleasedEvent);
	CONTROL_EVENT_MAP(RadioButton, RadioButtonIsCheckedChangedEvent);
	CONTROL_EVENT_MAP(Viewport, CharEvent);
	CONTROL_EVENT_MAP(Viewport, KeyPressedEvent);
	CONTROL_EVENT_MAP(Viewport, KeyReleasedEvent);
	CONTROL_EVENT_MAP(Viewport, MouseMoveEvent);
	CONTROL_EVENT_MAP(Viewport, MouseButtonPressedEvent);
	CONTROL_EVENT_MAP(Viewport, MouseButtonReleasedEvent);
	CONTROL_EVENT_MAP(Viewport, MouseScrolledEvent);
	CONTROL_EVENT_MAP(Viewport, MouseButtonDoubleClickEvent);


	std::unordered_map<std::string, std::function<void(Control*, const Timer&)>> m_onUpdateCallbacksMap;

	inline std::function<void(Control*, const Timer&)> GetOnUpdateCallbackImpl(const std::string& key)
	{
		return m_onUpdateCallbacksMap[key];
	}
	inline void AddOnUpdateCallbackImpl(const std::string& key, std::function<void(Control*, const Timer&)> fn)
	{
		m_onUpdateCallbacksMap[key] = fn;
	}



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

	void AddLayoutCallbackImpl(const std::string& key, LayoutCallbackFn fn) noexcept { m_layoutCallbacks[key] = fn; }
	LayoutCallbackFn GetLayoutCallbackImpl(const std::string& key) noexcept { return m_layoutCallbacks[key]; }

	Control* LoadControlImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);
	std::unique_ptr<Style> LoadStyleImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, json& data, const std::string& stylename);

	static std::unique_ptr<ColorBrush> LoadSolidColorBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	static std::unique_ptr<ColorBrush> LoadGradientBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	static std::unique_ptr<ColorBrush> LoadRadialBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	static std::unique_ptr<ColorBrush> LoadBitmapBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);

	void LoadControlsFromFileImpl(const std::string& fileName, Layout* parentLayout, std::optional<RowColumnPosition> rowColumnPositionOverride);

	bool LoadUIImpl(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept;
	void LoadGlobalStyles(std::shared_ptr<DeviceResources> deviceResources);
	void LoadLayoutDetails(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);
	void LoadPanes(std::shared_ptr<DeviceResources> deviceResources, Layout* layout);


	void LoadLayoutBrush(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);
	void LoadLayoutRowDefinitions(Layout* layout, json& data);
	void LoadLayoutColumnDefinitions(Layout* layout, json& data);
	void LoadLayoutMargin(Layout* layout, json& data);
	void LoadLayoutBorder(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);
	void LoadLayoutCallbacks(Layout* layout, json& data);
	void LoadLayoutID(Layout* layout, json& data);
	void LoadSubLayout(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name);

	RowColumnPosition ParseRowColumnPosition(json& data);
	std::tuple<RowColumnType, float> ParseRowColumnTypeAndSizeImpl(json& data, const std::string& layoutName);

	void ImportJSONImpl(json& data);

	bool IsControlKeyImpl(const std::string& controlKey) const noexcept { return m_controlLoaders.find(controlKey) != m_controlLoaders.end(); }
	bool IsStyleKeyImpl(const std::string& styleKey) const noexcept { return m_styleLoaders.find(styleKey) != m_styleLoaders.end(); }

	void AddControlNameImpl(const std::string& name);


	std::unordered_map<std::string, ControlLoaderFn>	m_controlLoaders; 
	std::unordered_map<std::string, StyleLoaderFn>		m_styleLoaders;
	std::unordered_map<std::string, LayoutCallbackFn>	m_layoutCallbacks;

	// Keep a cache of styles that have been parsed for quick lookup
	std::unordered_map<std::string, std::unique_ptr<Style>> m_stylesCache;

	// Keep a list of all control names so that we can ensure control names are unique
	std::vector<std::string> m_controlNames;

	json					m_jsonRoot;
	std::filesystem::path	m_jsonRootDirectory;
};
#pragma warning( pop )

}