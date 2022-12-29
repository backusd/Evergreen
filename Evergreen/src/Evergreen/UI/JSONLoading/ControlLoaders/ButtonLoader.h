#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/Button.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ButtonLoader : public ControlLoader
{
public:
	ButtonLoader(const ButtonLoader&) = delete;
	void operator=(const ButtonLoader&) = delete;
	~ButtonLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, const json& data, const std::string& name) { return std::move(Get().LoadImpl(deviceResources, parent, data, name)); }

private:
	ButtonLoader() noexcept = default;

	static ButtonLoader& Get() noexcept
	{
		static ButtonLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, const json& data, const std::string& name);

	std::unique_ptr<ColorBrush> ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data);
	std::unique_ptr<ColorBrush> ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data);
	float ParseBorderWidth(const json& data);
	void ParseContent(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, const json& data);

	void ParseOnMouseEnter(Button* button, const json& data);
	void ParseOnMouseLeave(Button* button, const json& data);
	void ParseOnMouseLButtonDown(Button* button, const json& data);
	void ParseOnMouseLButtonUp(Button* button, const json& data);
	void ParseOnClick(Button* button, const json& data);

};
#pragma warning( pop )

}