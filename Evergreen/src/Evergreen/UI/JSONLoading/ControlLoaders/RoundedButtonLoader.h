#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/RoundedButton.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API RoundedButtonLoader : public ControlLoader
{
public:
	RoundedButtonLoader(const RoundedButtonLoader&) = delete;
	void operator=(const RoundedButtonLoader&) = delete;
	~RoundedButtonLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadImpl(deviceResources, parent, data, name, rowColumnPositionOverride); }

private:
	RoundedButtonLoader() noexcept = default;

	static RoundedButtonLoader& Get() noexcept
	{
		static RoundedButtonLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);

	std::unique_ptr<ColorBrush> ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	float ParseBorderWidth(json& data);
	float ParseRadiusX(json& data);
	float ParseRadiusY(json& data);
	void ParseContent(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);

	void ParseOnMouseEntered(RoundedButton* button, json& data);
	void ParseOnMouseExited(RoundedButton* button, json& data);
	void ParseOnMouseMoved(RoundedButton* button, json& data);
	void ParseOnMouseLButtonDown(RoundedButton* button, json& data);
	void ParseOnMouseLButtonUp(RoundedButton* button, json& data);
	void ParseOnClick(RoundedButton* button, json& data);

};
#pragma warning( pop )

}