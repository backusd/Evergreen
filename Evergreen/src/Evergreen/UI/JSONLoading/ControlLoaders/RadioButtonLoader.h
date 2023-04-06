#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/RadioButton.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API RadioButtonLoader : public ControlLoader
{
public:
	RadioButtonLoader(const RadioButtonLoader&) = delete;
	void operator=(const RadioButtonLoader&) = delete;
	~RadioButtonLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadImpl(deviceResources, parent, data, name, rowColumnPositionOverride); }

private:
	RadioButtonLoader() noexcept = default;

	static RadioButtonLoader& Get() noexcept
	{
		static RadioButtonLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);

	ND bool ParseIsChecked(json& data);
	ND float ParseInnerRadius(json& data);
	ND float ParseOuterRadius(json& data);
	ND std::unique_ptr<ColorBrush> ParseInnerBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	ND std::unique_ptr<ColorBrush> ParseOuterBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	
	void ParseOuterLineWidth(RadioButton* rb, json& data);

	void ParseOnMouseEntered(RadioButton* rb, json& data);
	void ParseOnMouseExited(RadioButton* rb, json& data);
	void ParseOnMouseMoved(RadioButton* rb, json& data);
	void ParseOnMouseLButtonDown(RadioButton* rb, json& data);
	void ParseOnMouseLButtonUp(RadioButton* rb, json& data);
	void ParseOnIsCheckedChanged(RadioButton* rb, json& data);

};
#pragma warning( pop )

}