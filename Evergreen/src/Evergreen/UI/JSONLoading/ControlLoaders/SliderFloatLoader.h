#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/SliderFloat.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API SliderFloatLoader : public ControlLoader
{
public:
	SliderFloatLoader(const SliderFloatLoader&) = delete;
	void operator=(const SliderFloatLoader&) = delete;
	~SliderFloatLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadImpl(deviceResources, parent, data, name, rowColumnPositionOverride); }

private:
	SliderFloatLoader() noexcept = default;

	static SliderFloatLoader& Get() noexcept
	{
		static SliderFloatLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);

	float ParseMinimumValue(json& data);
	float ParseMaximumValue(json& data);
	float ParseValue(json& data);

	void ParseLineWidth(SliderFloat* slider, json& data);
	void ParseLineBrushRight(SliderFloat* slider, json& data);
	void ParseLineBrushLeft(SliderFloat* slider, json& data);
	void ParseFillLineOnRightSide(SliderFloat* slider, json& data);

	void ParseCircleRadius(SliderFloat* slider, json& data);
	void ParseCircleRadiusOuter(SliderFloat* slider, json& data);
	void ParseCircleBrush(SliderFloat* slider, json& data);
	void ParseCircleBrushOuter(SliderFloat* slider, json& data);

	void ParseValueFormatString(SliderFloat* slider, json& data);
	void ParseShowMinMaxTextValues(SliderFloat* slider, json& data);
	void ParseMinTextXYOffset(SliderFloat* slider, json& data);
	void ParseMinTextBrush(SliderFloat* slider, json& data);
	void ParseMinTextStyle(SliderFloat* slider, json& data);

	void ParseMaxTextXYOffset(SliderFloat* slider, json& data);
	void ParseMaxTextBrush(SliderFloat* slider, json& data);
	void ParseMaxTextStyle(SliderFloat* slider, json& data);

	void ParseMarginRightOfSlider(SliderFloat* slider, json& data);
	void ParseShowTextInputOnRight(SliderFloat* slider, json& data);
	void ParseTextInputHeight(SliderFloat* slider, json& data);
	void ParseTextInputWidth(SliderFloat* slider, json& data);
	void ParseTextInputTextBrush(SliderFloat* slider, json& data);
	void ParseTextInputTextStyle(SliderFloat* slider, json& data);
	void ParseTextInputBackgroundBrush(SliderFloat* slider, json& data);
	void ParseTextInputBorderBrush(SliderFloat* slider, json& data);
	void ParseTextInputBorderWidth(SliderFloat* slider, json& data);

	void ParseShowPopUpValueWhenSliding(SliderFloat* slider, json& data);
	void ParsePopUpBackgroundBrush(SliderFloat* slider, json& data);
	void ParsePopUpBorderBrush(SliderFloat* slider, json& data);
	void ParsePopUpBorderWidth(SliderFloat* slider, json& data);
	void ParsePopUpCornerRadius(SliderFloat* slider, json& data);
	void ParsePopUpHeight(SliderFloat* slider, json& data);
	void ParsePopUpWidth(SliderFloat* slider, json& data);
	void ParsePopUpTextBrush(SliderFloat* slider, json& data);
	void ParsePopUpTextStyle(SliderFloat* slider, json& data);

	void ParseOnMouseEnteredCircle(SliderFloat* slider, json& data);
	void ParseOnMouseExitedCircle(SliderFloat* slider, json& data);
	void ParseOnBeginDragging(SliderFloat* slider, json& data);
	void ParseOnStoppedDragging(SliderFloat* slider, json& data);
	void ParseOnValueChanged(SliderFloat* slider, json& data);

};
#pragma warning( pop )

}