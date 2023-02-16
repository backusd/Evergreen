#include "pch.h"
#include "SliderFloatLoader.h"

namespace Evergreen
{
Control* SliderFloatLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique


	RowColumnPosition rowCol;
	Margin margin;

	// Parse Row/Column
	rowCol = ParseRowColumnPosition(data);

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
	"PopUpHeight", "PopUpWidth", "PopUpTextBrush", "PopUpTextStyle",
	"OnMouseEnteredCircle", "OnMouseExitedCircle", "OnBeginDragging", "OnStoppedDragging", "OnValueChanged" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - SliderFloat control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	SliderFloat* slider = parent->CreateControl<SliderFloat>(
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

	ParseOnMouseEnteredCircle(slider, data);
	ParseOnMouseExitedCircle(slider, data);
	ParseOnBeginDragging(slider, data);
	ParseOnStoppedDragging(slider, data);
	ParseOnValueChanged(slider, data);

	return slider;
}

float SliderFloatLoader::ParseMinimumValue(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("MinimumValue"), "SliderFloat control with name '{}': 'MinimumValue' field is required. Incomplete SliderFloat object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["MinimumValue"].is_number(), "SliderFloat control with name '{}': 'MinimumValue' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
	return data["MinimumValue"].get<float>();
}
float SliderFloatLoader::ParseMaximumValue(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("MaximumValue"), "SliderFloat control with name '{}': 'MaximumValue' field is required. Incomplete SliderFloat object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["MaximumValue"].is_number(), "SliderFloat control with name '{}': 'MaximumValue' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
	return data["MaximumValue"].get<float>();
}
float SliderFloatLoader::ParseValue(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Value"), "SliderFloat control with name '{}': 'Value' field is required. Incomplete SliderFloat object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Value"].is_number(), "SliderFloat control with name '{}': 'Value' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
	return data["Value"].get<float>();
}

void SliderFloatLoader::ParseLineWidth(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("LineWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["LineWidth"].is_number(), "SliderFloat control with name '{}': 'LineWidth' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float width = data["LineWidth"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "SliderFloat control with name '{}': 'LineWidth' value ({}) must be >= 0. Invalid SliderFloat object: {}", m_name, width, data.dump(4));
		slider->SetLineWidth(width);
	}
}
void SliderFloatLoader::ParseLineBrushRight(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("LineBrushLeft"))
	{
		slider->SetLineBrushLeft(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["LineBrushLeft"])));
	}
}
void SliderFloatLoader::ParseLineBrushLeft(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("LineBrushRight"))
	{
		slider->SetLineBrushRight(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["LineBrushRight"])));
	}
}
void SliderFloatLoader::ParseFillLineOnRightSide(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("FillLineOnRightSide"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["FillLineOnRightSide"].is_boolean(), "SliderFloat control with name '{}': 'FillLineOnRightSide' value must be a boolean. Invalid SliderFloat object: {}", m_name, data.dump(4));
		slider->SetFillLineOnRightSide(data["FillLineOnRightSide"].get<bool>());
	}
}

void SliderFloatLoader::ParseCircleRadius(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("CircleRadius"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CircleRadius"].is_number(), "SliderFloat control with name '{}': 'CircleRadius' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float radius = data["CircleRadius"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radius > 0.0f, "SliderFloat control with name '{}': 'CircleRadius' value ({}) must be > 0. Invalid SliderFloat object: {}", m_name, radius, data.dump(4));
		slider->SetCircleRadius(radius);
	}
}
void SliderFloatLoader::ParseCircleRadiusOuter(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("CircleRadiusOuter"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CircleRadiusOuter"].is_number(), "SliderFloat control with name '{}': 'CircleRadiusOuter' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float radius = data["CircleRadiusOuter"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radius > 0.0f, "SliderFloat control with name '{}': 'CircleRadiusOuter' value ({}) must be > 0. Invalid SliderFloat object: {}", m_name, radius, data.dump(4));
		slider->SetCircleRadiusOuter(radius);
	}
}
void SliderFloatLoader::ParseCircleBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("CircleBrush"))
	{
		slider->SetCircleBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["CircleBrush"])));
	}
}
void SliderFloatLoader::ParseCircleBrushOuter(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("CircleBrushOuter"))
	{
		slider->SetCircleBrushOuter(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["CircleBrushOuter"])));
	}
}

void SliderFloatLoader::ParseValueFormatString(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("ValueFormatString"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ValueFormatString"].is_string(), "SliderFloat control with name '{}': 'ValueFormatString' value must be a string. Invalid SliderFloat object: {}", m_name, data.dump(4));
		std::string fmt = data["ValueFormatString"].get<std::string>();
		JSON_LOADER_EXCEPTION_IF_FALSE(fmt.size() > 0, "SliderFloat control with name '{}': 'ValueFormatString' string must not be empty. Invalid SliderFloat object: {}", m_name, data.dump(4));
		std::wstring format(fmt.begin(), fmt.end());
		slider->SetValueFormatString(format);
	}
}
void SliderFloatLoader::ParseShowMinMaxTextValues(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("ShowMinMaxTextValues"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ShowMinMaxTextValues"].is_boolean(), "SliderFloat control with name '{}': 'ShowMinMaxTextValues' value must be a boolean. Invalid SliderFloat object: {}", m_name, data.dump(4));
		slider->SetShowMinMaxTextValues(data["ShowMinMaxTextValues"].get<bool>());
	}
}
void SliderFloatLoader::ParseMinTextXYOffset(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("MinTextXYOffset"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["MinTextXYOffset"].is_array(), "SliderFloat control with name '{}': 'MinTextXYOffset' value must be an array. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["MinTextXYOffset"].size() == 2, "SliderFloat control with name '{}': The 'MinTextXYOffset' array must contain exactly two values. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["MinTextXYOffset"][0].is_number() && data["MinTextXYOffset"][1].is_number(), "SliderFloat control with name '{}': The 'MinTextXYOffset' array values must be numbers. Invalid SliderFloat object: {}", m_name, data.dump(4));
		slider->SetMinTextXOffset(data["MinTextXYOffset"][0].get<float>());
		slider->SetMinTextYOffset(data["MinTextXYOffset"][1].get<float>());
	}
}
void SliderFloatLoader::ParseMinTextBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("MinTextBrush"))
	{
		slider->SetMinTextBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["MinTextBrush"])));
	}

}
void SliderFloatLoader::ParseMinTextStyle(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");

	// Must parse out the name of the TextStyle before you can call JSONLoaders::LoadStyle
	std::string stylename;
	if (data.contains("MinTextStyle"))
	{
		if (data["MinTextStyle"].is_string())
		{
			stylename = data["MinTextStyle"].get<std::string>();
			JSON_LOADER_EXCEPTION_IF_FALSE(stylename.size() > 0, "SliderFloat control with name '{}': 'MinTextStyle' field cannot be an empty string.", m_name);
		}
		else if (data["MinTextStyle"].is_object())
		{
			stylename = m_name + "_MinValue_TextStyle";
		}
		else
		{
			JSON_LOADER_EXCEPTION("SliderFloat control with name '{}': 'MinTextStyle' field must be a string or an object. Invalid value: {}", m_name, data["MinTextStyle"].dump(4));
		}

		std::unique_ptr<Style> style = JSONLoaders::LoadStyle(slider->GetDeviceResources(), "TextStyle", data["MinTextStyle"], stylename);
		EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");
		std::unique_ptr<TextStyle> textStyle = std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(style.release())));
		// Force the text to be left aligned and vertically at the top
		textStyle->TextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		textStyle->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		slider->SetMinTextStyle(std::move(textStyle));
	}
}

void SliderFloatLoader::ParseMaxTextXYOffset(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("MaxTextXYOffset"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["MaxTextXYOffset"].is_array(), "SliderFloat control with name '{}': 'MaxTextXYOffset' value must be an array. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["MaxTextXYOffset"].size() == 2, "SliderFloat control with name '{}': The 'MaxTextXYOffset' array must contain exactly two values. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["MaxTextXYOffset"][0].is_number() && data["MaxTextXYOffset"][1].is_number(), "SliderFloat control with name '{}': The 'MaxTextXYOffset' array values must be numbers. Invalid SliderFloat object: {}", m_name, data.dump(4));
		slider->SetMaxTextXOffset(data["MaxTextXYOffset"][0].get<float>());
		slider->SetMaxTextYOffset(data["MaxTextXYOffset"][1].get<float>());
	}
}
void SliderFloatLoader::ParseMaxTextBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("MaxTextBrush"))
	{
		slider->SetMaxTextBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["MaxTextBrush"])));
	}
}
void SliderFloatLoader::ParseMaxTextStyle(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");

	// Must parse out the name of the TextStyle before you can call JSONLoaders::LoadStyle
	std::string stylename;
	if (data.contains("MaxTextStyle"))
	{
		if (data["MaxTextStyle"].is_string())
		{
			stylename = data["MaxTextStyle"].get<std::string>();
			JSON_LOADER_EXCEPTION_IF_FALSE(stylename.size() > 0, "SliderFloat control with name '{}': 'MaxTextStyle' field cannot be an empty string.", m_name);
		}
		else if (data["MaxTextStyle"].is_object())
		{
			stylename = m_name + "_MaxValue_TextStyle";
		}
		else
		{
			JSON_LOADER_EXCEPTION("SliderFloat control with name '{}': 'MaxTextStyle' field must be a string or an object. Invalid value: {}", m_name, data["MaxTextStyle"].dump(4));
		}

		std::unique_ptr<Style> style = JSONLoaders::LoadStyle(slider->GetDeviceResources(), "TextStyle", data["MaxTextStyle"], stylename);
		EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");
		std::unique_ptr<TextStyle> textStyle = std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(style.release())));
		// Force the text to be right aligned and vertically at the top
		textStyle->TextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		textStyle->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		slider->SetMaxTextStyle(std::move(textStyle));
	}
}

void SliderFloatLoader::ParseMarginRightOfSlider(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("MarginRightOfSlider"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["MarginRightOfSlider"].is_number(), "SliderFloat control with name '{}': 'MarginRightOfSlider' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float margin = data["MarginRightOfSlider"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(margin >= 0.0f, "SliderFloat control with name '{}': 'MarginRightOfSlider' value ({}) must be >= 0. Invalid SliderFloat object: {}", m_name, margin, data.dump(4));
		slider->SetMarginRightOfSlider(margin);
	}
}
void SliderFloatLoader::ParseShowTextInputOnRight(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("ShowTextInputOnRight"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ShowTextInputOnRight"].is_boolean(), "SliderFloat control with name '{}': 'ShowTextInputOnRight' value must be a boolean. Invalid SliderFloat object: {}", m_name, data.dump(4));
		slider->SetShowValueRightOfSlider(data["ShowTextInputOnRight"].get<bool>());
	}
}
void SliderFloatLoader::ParseTextInputHeight(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("TextInputHeight"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["TextInputHeight"].is_number(), "SliderFloat control with name '{}': 'TextInputHeight' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float height = data["TextInputHeight"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(height > 0.0f, "SliderFloat control with name '{}': 'TextInputHeight' value ({}) must be > 0. Invalid SliderFloat object: {}", m_name, height, data.dump(4));
		slider->SetTextInputHeight(height);
	}
}
void SliderFloatLoader::ParseTextInputWidth(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("TextInputWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["TextInputWidth"].is_number(), "SliderFloat control with name '{}': 'TextInputWidth' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float width = data["TextInputWidth"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(width > 0.0f, "SliderFloat control with name '{}': 'TextInputWidth' value ({}) must be > 0. Invalid SliderFloat object: {}", m_name, width, data.dump(4));
		slider->SetTextInputWidth(width);
	}
}
void SliderFloatLoader::ParseTextInputTextBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("TextInputTextBrush"))
	{
		slider->SetTextInputTextBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["TextInputTextBrush"])));
	}
}
void SliderFloatLoader::ParseTextInputTextStyle(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");

	// Must parse out the name of the TextStyle before you can call JSONLoaders::LoadStyle
	std::string stylename;
	if (data.contains("TextInputTextStyle"))
	{
		if (data["TextInputTextStyle"].is_string())
		{
			stylename = data["TextInputTextStyle"].get<std::string>();
			JSON_LOADER_EXCEPTION_IF_FALSE(stylename.size() > 0, "SliderFloat control with name '{}': 'TextInputTextStyle' field cannot be an empty string.", m_name);
		}
		else if (data["TextInputTextStyle"].is_object())
		{
			stylename = m_name + "_TextInput_TextStyle";
		}
		else
		{
			JSON_LOADER_EXCEPTION("SliderFloat control with name '{}': 'TextInputTextStyle' field must be a string or an object. Invalid value: {}", m_name, data["TextInputTextStyle"].dump(4));
		}

		std::unique_ptr<Style> style = JSONLoaders::LoadStyle(slider->GetDeviceResources(), "TextStyle", data["TextInputTextStyle"], stylename);
		EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");
		std::unique_ptr<TextStyle> textStyle = std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(style.release())));
		// Force the text to be left aligned and vertically centered
		textStyle->TextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		textStyle->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		slider->SetTextInputTextStyle(std::move(textStyle));
	}
}
void SliderFloatLoader::ParseTextInputBackgroundBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("TextInputBackgroundBrush"))
	{
		slider->SetTextInputBackgroundBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["TextInputBackgroundBrush"])));
	}
}
void SliderFloatLoader::ParseTextInputBorderBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("TextInputBorderBrush"))
	{
		slider->SetTextInputBorderBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["TextInputBorderBrush"])));
	}
}
void SliderFloatLoader::ParseTextInputBorderWidth(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("TextInputBorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["TextInputBorderWidth"].is_number(), "SliderFloat control with name '{}': 'TextInputBorderWidth' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float width = data["TextInputBorderWidth"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "SliderFloat control with name '{}': 'TextInputBorderWidth' value ({}) must be >= 0. Invalid SliderFloat object: {}", m_name, width, data.dump(4));
		slider->SetTextInputBorderWidth(width);
	}
}

void SliderFloatLoader::ParseShowPopUpValueWhenSliding(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("ShowPopUpValueWhenSliding"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ShowPopUpValueWhenSliding"].is_boolean(), "SliderFloat control with name '{}': 'ShowPopUpValueWhenSliding' value must be a boolean. Invalid SliderFloat object: {}", m_name, data.dump(4));
		slider->SetShowValueAsPopUpWhenSliding(data["ShowPopUpValueWhenSliding"].get<bool>());
	}
}
void SliderFloatLoader::ParsePopUpBackgroundBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("PopUpBackgroundBrush"))
	{
		slider->SetPopUpBackgroundBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["PopUpBackgroundBrush"])));
	}
}
void SliderFloatLoader::ParsePopUpBorderBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("PopUpBorderBrush"))
	{
		slider->SetPopUpBorderBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["PopUpBorderBrush"])));
	}
}
void SliderFloatLoader::ParsePopUpBorderWidth(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("PopUpBorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["PopUpBorderWidth"].is_number(), "SliderFloat control with name '{}': 'PopUpBorderWidth' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float width = data["PopUpBorderWidth"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "SliderFloat control with name '{}': 'PopUpBorderWidth' value ({}) must be >= 0. Invalid SliderFloat object: {}", m_name, width, data.dump(4));
		slider->SetPopUpBorderWidth(width);
	}
}
void SliderFloatLoader::ParsePopUpCornerRadius(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("PopUpCornerRadius"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("PopUpCornerRadiusX"), "SliderFloat control with name '{}': When 'PopUpCornerRadius' field is used, it is invalid to also include 'CornerRadiusX'. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("PopUpCornerRadiusY"), "SliderFloat control with name '{}': When 'PopUpCornerRadius' field is used, it is invalid to also include 'CornerRadiusY'. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["PopUpCornerRadius"].is_number(), "SliderFloat control with name '{}': 'PopUpCornerRadius' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float radius = data["PopUpCornerRadius"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radius >= 0.0f, "SliderFloat control with name '{}': 'PopUpCornerRadius' must be greater than or equal to 0. Invalid SliderFloat object: {}", m_name, data.dump(4));
		slider->SetPopUpCornerRadius(radius);
	}
	else if (data.contains("PopUpCornerRadiusX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("PopUpCornerRadiusY"), "SliderFloat control with name '{}': When 'PopUpCornerRadiusX' field is used, 'PopUpCornerRadiusY' is also required. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["PopUpCornerRadiusX"].is_number(), "SliderFloat control with name '{}': 'PopUpCornerRadiusX' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["PopUpCornerRadiusY"].is_number(), "SliderFloat control with name '{}': 'PopUpCornerRadiusY' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float radiusX = data["PopUpCornerRadiusX"].get<float>();
		float radiusY = data["PopUpCornerRadiusY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radiusX >= 0.0f, "SliderFloat control with name '{}': 'PopUpCornerRadiusX' must be greater than or equal to 0. Invalid SliderFloat object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(radiusY >= 0.0f, "SliderFloat control with name '{}': 'PopUpCornerRadiusY' must be greater than or equal to 0. Invalid SliderFloat object: {}", m_name, data.dump(4));
		slider->SetPopUpCornerRadius(radiusX, radiusY);
	}
	else
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("PopUpCornerRadiusY"), "Pane control with name '{}': When 'PopUpCornerRadiusY' field is used, 'PopUpCornerRadiusX' is also required. Invalid SliderFloat object: {}", m_name, data.dump(4));
	}
}
void SliderFloatLoader::ParsePopUpHeight(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("PopUpHeight"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["PopUpHeight"].is_number(), "SliderFloat control with name '{}': 'PopUpHeight' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float height = data["PopUpHeight"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(height > 0.0f, "SliderFloat control with name '{}': 'PopUpHeight' value ({}) must be > 0. Invalid SliderFloat object: {}", m_name, height, data.dump(4));
		slider->SetPopUpHeight(height);
	}
}
void SliderFloatLoader::ParsePopUpWidth(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("PopUpWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["PopUpWidth"].is_number(), "SliderFloat control with name '{}': 'PopUpWidth' value must be a number. Invalid SliderFloat object: {}", m_name, data.dump(4));
		float width = data["PopUpWidth"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(width > 0.0f, "SliderFloat control with name '{}': 'PopUpWidth' value ({}) must be > 0. Invalid SliderFloat object: {}", m_name, width, data.dump(4));
		slider->SetPopUpWidth(width);
	}
}
void SliderFloatLoader::ParsePopUpTextBrush(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("PopUpTextBrush"))
	{
		slider->SetPopUpTextBrush(std::move(JSONLoaders::LoadBrush(slider->GetDeviceResources(), data["PopUpTextBrush"])));
	}
}
void SliderFloatLoader::ParsePopUpTextStyle(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");

	// Must parse out the name of the TextStyle before you can call JSONLoaders::LoadStyle
	std::string stylename;
	if (data.contains("PopUpTextStyle"))
	{
		if (data["PopUpTextStyle"].is_string())
		{
			stylename = data["PopUpTextStyle"].get<std::string>();
			JSON_LOADER_EXCEPTION_IF_FALSE(stylename.size() > 0, "SliderFloat control with name '{}': 'PopUpTextStyle' field cannot be an empty string.", m_name);
		}
		else if (data["PopUpTextStyle"].is_object())
		{
			stylename = m_name + "_PopUp_TextStyle";
		}
		else
		{
			JSON_LOADER_EXCEPTION("SliderFloat control with name '{}': 'PopUpTextStyle' field must be a string or an object. Invalid value: {}", m_name, data["PopUpTextStyle"].dump(4));
		}

		std::unique_ptr<Style> style = JSONLoaders::LoadStyle(slider->GetDeviceResources(), "TextStyle", data["PopUpTextStyle"], stylename);
		EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");
		std::unique_ptr<TextStyle> textStyle = std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(style.release())));
		// Force the text to be centered horizontally and vertically
		textStyle->TextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		textStyle->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		slider->SetPopUpTextStyle(std::move(textStyle));
	}
}

void SliderFloatLoader::ParseOnMouseEnteredCircle(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("OnMouseEnteredCircle"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseEnteredCircle"].is_string(), "SliderFloat control with name '{}': 'OnMouseEnteredCircle' value must be a string. Invalid SliderFloat object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseEnteredCircle"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<SliderFloat, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "SliderFloat control with name '{}': 'OnMouseEnteredCircle' callback not found for key '{}'. Invalid SliderFloat object: {}", m_name, key, data.dump(4));
		slider->SetOnMouseEnteredCircleCallback(callback);
	}
}
void SliderFloatLoader::ParseOnMouseExitedCircle(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("OnMouseExitedCircle"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseExitedCircle"].is_string(), "SliderFloat control with name '{}': 'OnMouseExitedCircle' value must be a string. Invalid SliderFloat object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseExitedCircle"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<SliderFloat, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "SliderFloat control with name '{}': 'OnMouseExitedCircle' callback not found for key '{}'. Invalid SliderFloat object: {}", m_name, key, data.dump(4));
		slider->SetOnMouseExitedCircleCallback(callback);
	}
}
void SliderFloatLoader::ParseOnBeginDragging(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("OnBeginDragging"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnBeginDragging"].is_string(), "SliderFloat control with name '{}': 'OnBeginDragging' value must be a string. Invalid SliderFloat object: {}", m_name, data.dump(4));
		std::string key = data["OnBeginDragging"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<SliderFloat, MouseButtonPressedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "SliderFloat control with name '{}': 'SetOnBeginDraggingCallback' callback not found for key '{}'. Invalid SliderFloat object: {}", m_name, key, data.dump(4));
		slider->SetOnBeginDraggingCallback(callback);
	}
}
void SliderFloatLoader::ParseOnStoppedDragging(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("OnStoppedDragging"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnStoppedDragging"].is_string(), "SliderFloat control with name '{}': 'OnStoppedDragging' value must be a string. Invalid SliderFloat object: {}", m_name, data.dump(4));
		std::string key = data["OnStoppedDragging"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<SliderFloat, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "SliderFloat control with name '{}': 'OnStoppedDragging' callback not found for key '{}'. Invalid SliderFloat object: {}", m_name, key, data.dump(4));
		slider->SetOnStoppedDraggingCallback(callback);
	}
}
void SliderFloatLoader::ParseOnValueChanged(SliderFloat* slider, json& data)
{
	EG_CORE_ASSERT(slider != nullptr, "No slider");
	if (data.contains("OnValueChanged"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnValueChanged"].is_string(), "SliderFloat control with name '{}': 'OnValueChanged' value must be a string. Invalid SliderFloat object: {}", m_name, data.dump(4));
		std::string key = data["OnValueChanged"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<SliderFloat, SliderFloatValueChangedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "SliderFloat control with name '{}': 'OnValueChanged' callback not found for key '{}'. Invalid SliderFloat object: {}", m_name, key, data.dump(4));
		slider->SetOnValueChangedCallback(callback);
	}
}
}