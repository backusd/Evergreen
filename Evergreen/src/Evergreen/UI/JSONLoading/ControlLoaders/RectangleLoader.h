#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/Rectangle.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API RectangleLoader : public ControlLoader
{
public:
	RectangleLoader(const RectangleLoader&) = delete;
	RectangleLoader& operator=(const RectangleLoader&) = delete;
	~RectangleLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadImpl(deviceResources, parent, data, name, rowColumnPositionOverride); }

private:
	RectangleLoader() noexcept = default;

	static RectangleLoader& Get() noexcept
	{
		static RectangleLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);

	std::unique_ptr<ColorBrush> ParseBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	float ParseHeight(json& data);
	float ParseWidth(json& data);
};
#pragma warning( pop )

}