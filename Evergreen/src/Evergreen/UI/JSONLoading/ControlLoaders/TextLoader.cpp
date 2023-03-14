#include "pch.h"
#include "TextLoader.h"
#include "Evergreen/UI/JSONLoading/StyleLoaders/TextStyleLoader.h"


namespace Evergreen
{
Control* TextLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique

	std::wstring text;
	RowColumnPosition rowCol;
	Margin margin;

	// Validate JSON (will throw if there is a problem)
	ValidateJSONData(data);

	// Parse Row/Column
	rowCol = ParseRowColumnPosition(data);

	// Parse Margin
	margin = ParseMargin(data);

	// Parse Text
	text = ParseText(data);

	// Parse Brush
	std::unique_ptr<ColorBrush> brush = ParseBrush(deviceResources, data);
	EG_CORE_ASSERT(brush != nullptr, "Not allowed to return nullptr. Should have thrown exception");

	// Parse Style
	std::unique_ptr<TextStyle> style = ParseStyle(deviceResources, data);
	EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");


	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Text", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"Style", "Brush", "FontFamily", "FontSize", "FontWeight", "FontStyle", "FontStretch", "TextAlignment",
	"ParagraphAlignment", "WordWrapping", "Trimming", "Locale", "OnUpdate"};
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Text control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	Text* textControl = parent->CreateControl<Text>(rowCol, deviceResources, text, std::move(brush), std::move(style), margin);
	EG_CORE_ASSERT(textControl != nullptr, "Something went wrong, Text is nullptr");

	textControl->Name(name);
	textControl->ID(ParseID(data));

	ParseOnUpdateCallback(textControl, data);
	
	return textControl;
}

void TextLoader::ValidateJSONData(json& data)
{
	// Because Text may contain TextStyle fields, make sure that if 'Style' is specified,
	// no other TextStyle fields are present
	if (data.contains("Style"))
	{
		constexpr std::array textStyleFields = {
			"FontFamily", "FontSize", "FontWeight", "FontStyle", "FontStretch", "TextAlignment",
			"ParagraphAlignment", "WordWrapping", "Trimming", "Locale"
		};

		for (auto& [key, value] : data.items())
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(std::find(textStyleFields.begin(), textStyleFields.end(), key) == textStyleFields.end(), "Invalid json for Text control with name '{}'. Cannot include '{}' field if 'Style' field is specified.", m_name, key);
		}
	}
}

std::wstring TextLoader::ParseText(json& data)
{
	std::wstring text = L"";

	if (data.contains("Text"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Text"].is_string(), "Text control with name '{}': 'Text' field must be a string. Invalid value: {}", m_name, data["Text"].dump(4));

		std::string parsedText = data["Text"].get<std::string>();
		std::wstring s = std::wstring(parsedText.begin(), parsedText.end());

		// If the text starts with '0x' parse it as a hex value
		if (s.size() > 1 && s[0] == L'0' && s[1] == L'x')
		{
			try
			{
				text.push_back(static_cast<wchar_t>(std::stoi(s, nullptr, 16)));
			}
			catch (const std::invalid_argument& e)
			{
				JSON_LOADER_EXCEPTION("Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw invalid_argument exception.\nException Description: {}", m_name, parsedText, e.what());
			}
			catch (const std::out_of_range& e)
			{
				JSON_LOADER_EXCEPTION("Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw out_of_range exception.\nException Description: {}", m_name, parsedText, e.what());
			}
		}
		else
		{
			text = std::move(s);
		}
	}

	return text;
}
std::unique_ptr<ColorBrush> TextLoader::ParseBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Brush"), "Text control with name '{}': No 'Brush' key. Incomplete Text object: {}", m_name, data.dump(4));

	return std::move(JSONLoaders::LoadBrush(deviceResources, data["Brush"]));
}
std::unique_ptr<TextStyle> TextLoader::ParseStyle(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// Must parse out the name of the TextStyle before you can call JSONLoaders::LoadStyle
	std::string stylename;
	if (data.contains("Style"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Style"].is_string(), "Text control with name '{}': 'Style' field must be a string. Invalid value: {}", m_name, data["Style"].dump(4));

		stylename = data["Style"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(stylename.size() > 0, "Text control with name '{}': 'Style' field cannot be an empty string.", m_name);
	}
	else
	{
		stylename = m_name + "_TextStyle";
	}

	std::unique_ptr<Style> style = JSONLoaders::LoadStyle(deviceResources, "TextStyle", data, stylename);
	EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");

	return std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(style.release())));
}



}