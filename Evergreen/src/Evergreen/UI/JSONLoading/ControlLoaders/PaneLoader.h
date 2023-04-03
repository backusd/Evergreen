#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/Pane.h"

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
	void operator=(const PaneLoader&) = delete;
	~PaneLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name) { return Get().LoadImpl(deviceResources, parent, data, name); }

private:
	PaneLoader() noexcept = default;

	static PaneLoader& Get() noexcept
	{
		static PaneLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name);

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
	
	void ParseOnMouseEnteredTitleBar(Pane* pane, json& data);
	void ParseOnMouseExitedTitleBar(Pane* pane, json& data);
	void ParseOnMouseEnteredContentRegion(Pane* pane, json& data);
	void ParseOnMouseExitedContentRegion(Pane* pane, json& data);
	void ParseOnMouseMoved(Pane* pane, json& data);

};
#pragma warning( pop )

}