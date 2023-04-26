#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/Pane.h"
#include "Evergreen/UI/UI.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API PaneLoader : public ControlLoader
{
public:
	PaneLoader(const PaneLoader&) = delete;
	PaneLoader& operator=(const PaneLoader&) = delete;
	~PaneLoader() noexcept override {}

	template<typename T>
	ND static inline Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) requires (std::is_base_of_v<Pane, T>)
	{
		return Get().LoadImpl<T>(deviceResources, parent, data, name, rowColumnPositionOverride);
	}

private:
	PaneLoader() noexcept = default;

	static PaneLoader& Get() noexcept
	{
		static PaneLoader loader;
		return loader;
	}

	template<typename T>
	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<Pane, T>);

	// Pane is special in that row/column positioning and margin doesn't make sense
	// So if the user included them, just log a warning message and delete that piece of data
	void LogWarningForRowColumnPositionAndMarginKeywords(json& data);

	float ParseTop(json& data);
	float ParseLeft(json& data);
	float ParseHeight(json& data);
	float ParseWidth(json& data);
	bool ParseResizable(json& data);
	bool ParseRelocatable(json& data);
	std::unique_ptr<ColorBrush> ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::array<float, 4> ParseBorderWidth(json& data);
	bool ParseIncludeTitleBar(json& data);
	std::unique_ptr<ColorBrush> ParseTitleBarBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	float ParseTitleBarHeight(json& data);

	void ParseBorderOffsets(Pane* pane, json& data);

	void ParseCornerRadius(Pane* pane, json& data);
	void ParseIsMinimized(Pane* pane, json& data);
	void ParseIsVisible(Pane* pane, json& data);

	void ParseTitle(Pane* pane, json& data);
	void ParseContent(Pane* pane, json& data);

};
#pragma warning( pop )

template<typename T>
Control* PaneLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<Pane, T>)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	// NOTE: The pane will not be created as a child control of the parent layout. However, in order to keep the 
	// same function signature, we include a Layout* parameter. We are also going to require it not be nullptr
	// so that we can get the UI* which is how we will end up storing the created Pane
	EG_CORE_ASSERT(parent != nullptr, "No parent layout");
	UI* ui = parent->GetUI();
	EG_CORE_ASSERT(ui != nullptr, "No UI");

	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique

	// Pane is special in that row/column positioning and margin doesn't make sense
	// So if the user included them, just log a warning message and delete that piece of data
	LogWarningForRowColumnPositionAndMarginKeywords(data);

	float top = ParseTop(data);
	float left = ParseLeft(data);
	float height = ParseHeight(data);
	float width = ParseWidth(data);

	bool resizable = ParseResizable(data);
	bool relocatable = ParseRelocatable(data);

	// Parse Brushes
	std::unique_ptr<ColorBrush> backgroundBrush = ParseBackgroundBrush(deviceResources, data);
	std::unique_ptr<ColorBrush> borderBrush = ParseBorderBrush(deviceResources, data);

	// Border Width
	std::array<float, 4> borderWidths = ParseBorderWidth(data);

	// Title attributes
	bool includeTitleBar = ParseIncludeTitleBar(data);
	std::unique_ptr<ColorBrush> titleBarBrush = ParseTitleBarBrush(deviceResources, data);
	float titleBarHeight = ParseTitleBarHeight(data);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Text", "Title", "Top", "Left", "Height", "Width", "Resizable",
	"Relocatable", "BackgroundBrush", "BorderBrush", "BorderWidth", "CornerRadius", "CornerRadiusX", "CornerRadiusY",
	"IncludeTitleBar", "TitleBarBrush", "TitleBarHeight", "IsMinimized", "IsVisible", "Content",
	"BorderTopLeftOffsetX", "BorderTopLeftOffsetY", "BorderTopRightOffsetX", "BorderTopRightOffsetY", "BorderBottomLeftOffsetX",
	"BorderBottomLeftOffsetY", "BorderBottomRightOffsetX", "BorderBottomRightOffsetY" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Pane control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	std::unique_ptr<T> p = std::make_unique<T>(
		deviceResources,
		ui, // UI*
		top, // top
		left, // left
		height, // height
		width, // width
		resizable, // resizable 
		relocatable, // relocatable
		std::move(backgroundBrush), // background brush
		std::move(borderBrush), // border brush
		borderWidths, // border width
		includeTitleBar, // includeTitleBar
		std::move(titleBarBrush), // TitleBarBrush
		titleBarHeight // TitleBar height
	);
	EG_CORE_ASSERT(p != nullptr, "Something went wrong, pane is nullptr");
	Pane* pane = ui->AddPane<T>(std::move(p), name);
	EG_CORE_ASSERT(pane != nullptr, "Something went wrong, pane is nullptr");

	pane->Name(name);
	pane->ID(ParseID(data));

	ParseBorderOffsets(pane, data);

	ParseCornerRadius(pane, data);
	ParseIsMinimized(pane, data);
	ParseIsVisible(pane, data);

	ParseTitle(pane, data);
	ParseContent(pane, data);

	return pane;
}

}