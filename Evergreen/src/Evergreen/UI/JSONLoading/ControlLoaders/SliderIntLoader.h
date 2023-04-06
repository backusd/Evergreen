#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/SliderInt.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API SliderIntLoader : public ControlLoader
{
public:
	SliderIntLoader(const SliderIntLoader&) = delete;
	void operator=(const SliderIntLoader&) = delete;
	~SliderIntLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadImpl(deviceResources, parent, data, name, rowColumnPositionOverride); }

private:
	SliderIntLoader() noexcept = default;

	static SliderIntLoader& Get() noexcept
	{
		static SliderIntLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);

	int ParseMinimumValue(json& data);
	int ParseMaximumValue(json& data);
	int ParseValue(json& data);

	void ParseLineWidth(SliderInt* slider, json& data);
	void ParseLineBrushRight(SliderInt* slider, json& data);
	void ParseLineBrushLeft(SliderInt* slider, json& data);
	void ParseFillLineOnRightSide(SliderInt* slider, json& data);

	void ParseCircleRadius(SliderInt* slider, json& data);
	void ParseCircleRadiusOuter(SliderInt* slider, json& data);
	void ParseCircleBrush(SliderInt* slider, json& data);
	void ParseCircleBrushOuter(SliderInt* slider, json& data);

	void ParseValueFormatString(SliderInt* slider, json& data);
	void ParseShowMinMaxTextValues(SliderInt* slider, json& data);
	void ParseMinTextXYOffset(SliderInt* slider, json& data);
	void ParseMinTextBrush(SliderInt* slider, json& data);
	void ParseMinTextStyle(SliderInt* slider, json& data);

	void ParseMaxTextXYOffset(SliderInt* slider, json& data);
	void ParseMaxTextBrush(SliderInt* slider, json& data);
	void ParseMaxTextStyle(SliderInt* slider, json& data);

	void ParseMarginRightOfSlider(SliderInt* slider, json& data);
	void ParseShowTextInputOnRight(SliderInt* slider, json& data);
	void ParseTextInputHeight(SliderInt* slider, json& data);
	void ParseTextInputWidth(SliderInt* slider, json& data);
	void ParseTextInputTextBrush(SliderInt* slider, json& data);
	void ParseTextInputTextStyle(SliderInt* slider, json& data);
	void ParseTextInputBackgroundBrush(SliderInt* slider, json& data);
	void ParseTextInputBorderBrush(SliderInt* slider, json& data);
	void ParseTextInputBorderWidth(SliderInt* slider, json& data);

	void ParseShowPopUpValueWhenSliding(SliderInt* slider, json& data);
	void ParsePopUpBackgroundBrush(SliderInt* slider, json& data);
	void ParsePopUpBorderBrush(SliderInt* slider, json& data);
	void ParsePopUpBorderWidth(SliderInt* slider, json& data);
	void ParsePopUpCornerRadius(SliderInt* slider, json& data);
	void ParsePopUpHeight(SliderInt* slider, json& data);
	void ParsePopUpWidth(SliderInt* slider, json& data);
	void ParsePopUpTextBrush(SliderInt* slider, json& data);
	void ParsePopUpTextStyle(SliderInt* slider, json& data);

	void ParseOnMouseEnteredCircle(SliderInt* slider, json& data);
	void ParseOnMouseExitedCircle(SliderInt* slider, json& data);
	void ParseOnBeginDragging(SliderInt* slider, json& data);
	void ParseOnStoppedDragging(SliderInt* slider, json& data);
	void ParseOnValueChanged(SliderInt* slider, json& data);

};
#pragma warning( pop )

}