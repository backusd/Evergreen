#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/ScrollableLayout.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ScrollableLayoutLoader : public ControlLoader
{
public:
	ScrollableLayoutLoader(const ScrollableLayoutLoader&) = delete;
	void operator=(const ScrollableLayoutLoader&) = delete;
	~ScrollableLayoutLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name) { return Get().LoadImpl(deviceResources, parent, data, name); }

private:
	ScrollableLayoutLoader() noexcept = default;

	static ScrollableLayoutLoader& Get() noexcept
	{
		static ScrollableLayoutLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name);

	std::unique_ptr<ColorBrush> ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	float ParseBorderWidth(json& data);

	bool ParseScrollVertical(json& data);
	bool ParseScrollHorizontal(json& data);

	void ParseRowDefinitions(ScrollableLayout* scrollableLayout, json& data, bool canScrollVertical);
	void ParseColumnDefinitions(ScrollableLayout* scrollableLayout, json& data, bool canScrollHorizontal);

	void ParseVerticalScrollBarCornerRadius(ScrollableLayout* scrollableLayout, json& data);
	void ParseVerticalScrollBarEnabled(ScrollableLayout* scrollableLayout, json& data);
	void ParseVerticalScrollBarHiddenWhenNotOver(ScrollableLayout* scrollableLayout, json& data);
	void ParseVerticalScrollBarWidth(ScrollableLayout* scrollableLayout, json& data);
	void ParseVerticalScrollBarRegionWidth(ScrollableLayout* scrollableLayout, json& data);
	void ParseVerticalScrollBarBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data);
	void ParseVerticalScrollBarHoveredBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data);
	void ParseVerticalScrollBarDraggingBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data);

	void ParseHorizontalScrollBarCornerRadius(ScrollableLayout* scrollableLayout, json& data);
	void ParseHorizontalScrollBarEnabled(ScrollableLayout* scrollableLayout, json& data);
	void ParseHorizontalScrollBarHiddenWhenNotOver(ScrollableLayout* scrollableLayout, json& data);
	void ParseHorizontalScrollBarHeight(ScrollableLayout* scrollableLayout, json& data);
	void ParseHorizontalScrollBarRegionHeight(ScrollableLayout* scrollableLayout, json& data);
	void ParseHorizontalScrollBarBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data);
	void ParseHorizontalScrollBarHoveredBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data);
	void ParseHorizontalScrollBarDraggingBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data);

	void LoadControls(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data);

};
#pragma warning( pop )

}