#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/Viewport.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ViewportLoader : public ControlLoader
{
public:
	ViewportLoader(const ViewportLoader&) = delete;
	void operator=(const ViewportLoader&) = delete;
	~ViewportLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadImpl(deviceResources, parent, data, name, rowColumnPositionOverride); }

private:
	ViewportLoader() noexcept = default;

	static ViewportLoader& Get() noexcept
	{
		static ViewportLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);

	void ParseContent(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);

	void ParseOnChar(Viewport* vp, json& data);
	void ParseOnKeyPressed(Viewport* vp, json& data);
	void ParseOnKeyReleased(Viewport* vp, json& data);
	void ParseOnMouseEntered(Viewport* vp, json& data);
	void ParseOnMouseExited(Viewport* vp, json& data);
	void ParseOnMouseMoved(Viewport* vp, json& data);
	void ParseOnMouseButtonPressed(Viewport* vp, json& data);
	void ParseOnMouseButtonReleased(Viewport* vp, json& data);
	void ParseOnMouseScrolledVertical(Viewport* vp, json& data);
	void ParseOnMouseScrolledHorizontal(Viewport* vp, json& data);
	void ParseOnClick(Viewport* vp, json& data);
	void ParseOnDoubleClick(Viewport* vp, json& data);

};
#pragma warning( pop )

}