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
	SliderFloatLoader& operator=(const SliderFloatLoader&) = delete;
	~SliderFloatLoader() noexcept override {}

	template<typename T>
	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) requires (std::is_base_of_v<SliderFloat, T>)
	{ 
		return Get().LoadImpl<T>(deviceResources, parent, data, name, rowColumnPositionOverride); 
	}

private:
	SliderFloatLoader() noexcept = default;

	static SliderFloatLoader& Get() noexcept
	{
		static SliderFloatLoader loader;
		return loader;
	}

	template<typename T>
	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<SliderFloat, T>);

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

};
#pragma warning( pop )

template<typename T>
Control* SliderFloatLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<SliderFloat, T>)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique

	RowColumnPosition rowCol;
	Margin margin;

	// Parse Row/Column
	rowCol = rowColumnPositionOverride.has_value() ? rowColumnPositionOverride.value() : ParseRowColumnPosition(data);

	// Parse Margin
	margin = ParseMargin(data);

	// Parse Min/Max/Value
	float min = ParseMinimumValue(data);
	float max = ParseMaximumValue(data);
	float value = ParseValue(data);
	JSON_LOADER_EXCEPTION_IF_FALSE(min < max, "SliderFloat control with name '{}': 'MinimumValue' ({}) must be less than 'MaximumValue' ({}). Invalid SliderFloat object: {}", m_name, min, max, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(min <= value && value <= max, "SliderFloat control with name '{}': 'Value' ({}) must be >= 'MinimumValue' ({}) and <= 'MaximumValue' ({}). Invalid SliderFloat object: {}", m_name, value, min, max, data.dump(4));

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"MinimumValue",	"MaximumValue",	"Value",
	"LineWidth", "LineBrushLeft", "LineBrushRight", "FillLineOnRightSide",
	"CircleRadius", "CircleRadiusOuter", "CircleBrush", "CircleBrushOuter",
	"ValueFormatString", "ShowMinMaxTextValues", "MinTextXYOffset", "MinTextBrush", "MinTextStyle",
	"MaxTextXYOffset", "MaxTextBrush", "MaxTextStyle",
	"MarginRightOfSlider", "ShowTextInputOnRight", "TextInputHeight", "TextInputWidth", "TextInputTextBrush",
	"TextInputTextStyle", "TextInputBackgroundBrush", "TextInputBorderBrush", "TextInputBorderWidth",
	"ShowPopUpValueWhenSliding", "PopUpBackgroundBrush", "PopUpBorderBrush", "PopUpBorderWidth", "PopUpCornerRadius",
	"PopUpHeight", "PopUpWidth", "PopUpTextBrush", "PopUpTextStyle" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - SliderFloat control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	T* slider = parent->CreateControl<T>(
		rowCol,
		deviceResources,
		min,
		max,
		value,
		margin);
	EG_CORE_ASSERT(slider != nullptr, "Something went wrong, slider is nullptr");

	slider->Name(name);
	slider->ID(ParseID(data));

	ParseLineWidth(slider, data);
	ParseLineBrushRight(slider, data);
	ParseLineBrushLeft(slider, data);
	ParseFillLineOnRightSide(slider, data);

	ParseCircleRadius(slider, data);
	ParseCircleRadiusOuter(slider, data);
	ParseCircleBrush(slider, data);
	ParseCircleBrushOuter(slider, data);

	ParseValueFormatString(slider, data);
	ParseShowMinMaxTextValues(slider, data);
	ParseMinTextXYOffset(slider, data);
	ParseMinTextBrush(slider, data);
	ParseMinTextStyle(slider, data);

	ParseMaxTextXYOffset(slider, data);
	ParseMaxTextBrush(slider, data);
	ParseMaxTextStyle(slider, data);

	ParseMarginRightOfSlider(slider, data);
	ParseShowTextInputOnRight(slider, data);
	ParseTextInputHeight(slider, data);
	ParseTextInputWidth(slider, data);
	ParseTextInputTextBrush(slider, data);
	ParseTextInputTextStyle(slider, data);
	ParseTextInputBackgroundBrush(slider, data);
	ParseTextInputBorderBrush(slider, data);
	ParseTextInputBorderWidth(slider, data);

	ParseShowPopUpValueWhenSliding(slider, data);
	ParsePopUpBackgroundBrush(slider, data);
	ParsePopUpBorderBrush(slider, data);
	ParsePopUpBorderWidth(slider, data);
	ParsePopUpCornerRadius(slider, data);
	ParsePopUpHeight(slider, data);
	ParsePopUpWidth(slider, data);
	ParsePopUpTextBrush(slider, data);
	ParsePopUpTextStyle(slider, data);

	return slider;
}

}