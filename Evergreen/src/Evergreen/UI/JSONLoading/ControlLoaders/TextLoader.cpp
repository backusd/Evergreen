#include "pch.h"
#include "TextLoader.h"
#include "Evergreen/UI/JSONLoading/StyleLoaders/TextStyleLoader.h"


namespace Evergreen
{
Control* TextLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, const json& data, const std::string& name) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;

	std::wstring text = L"";
	RowColumnPosition rowCol;
	Margin margin{ 0 };

	// Validate JSON
	if (!ValidateJSONData(data))
		return nullptr;

	// Parse Row/Column
	if (std::optional<RowColumnPosition> rowColOpt = ParseRowColumnPosition(data))
		rowCol = rowColOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseRowColumnPosition() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	// Parse Margin
	if (std::optional<Margin> marginOpt = ParseMargin(data))
		margin = marginOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseRowColumnPosition() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	// Parse Text
	if (std::optional<std::wstring> textOpt = ParseText(data))
		text = textOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseText() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	// Parse Brush
	std::unique_ptr<ColorBrush> brush = ParseBrush(deviceResources, data);
	if (brush == nullptr)
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseBrush() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	// Parse Style
	std::shared_ptr<TextStyle> style = ParseStyle(deviceResources, data);
	if (style == nullptr)
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseStyle() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "Type", "Text", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"Style", "Brush", "FontFamily", "FontSize", "FontWeight", "FontStyle", "FontStretch", "TextAlignment",
	"ParagraphAlignment", "WordWrapping", "Trimming", "Locale"};
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Text control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	if (std::optional<Text*> textOpt = parent->AddControl<Text>(rowCol, deviceResources, text, std::move(brush), style, margin))
	{
		textOpt.value()->Name(name);
		return textOpt.value();
	}

	return nullptr;
}

bool TextLoader::ValidateJSONData(const json& data) noexcept
{
	// Because Text may contain TextStyle fields, make sure that if 'Style' is specified,
	// no other TextStyle fields are present
	if (data.contains("Style"))
	{
		constexpr std::array testStyleFields = {
			"FontFamily", "FontSize", "FontWeight", "FontStyle", "FontStretch", "TextAlignment",
			"ParagraphAlignment", "WordWrapping", "Trimming", "Locale"
		};

		for (auto& [key, value] : data.items())
		{
			if (std::find(testStyleFields.begin(), testStyleFields.end(), key) != testStyleFields.end())
			{
				EG_CORE_ERROR("{}:{} - Invalid json for Text control with name '{}'. Cannot include '{}' field if 'Style' field is specified.", __FILE__, __LINE__, m_name, key);
				return false;
			}
		}
	}

	return true;
}

std::optional<std::wstring> TextLoader::ParseText(const json& data) noexcept
{
	std::wstring text = L"";

	if (data.contains("Text"))
	{
		if (!data["Text"].is_string())
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Text' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["Text"].dump(4));
			return std::nullopt;
		}

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
				EG_CORE_ERROR("{}:{} - Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw invalid_argument exception.", __FILE__, __LINE__, m_name, parsedText);
				EG_CORE_ERROR("Exception Description: {}", e.what());
				return std::nullopt;
			}
			catch (const std::out_of_range& e)
			{
				EG_CORE_ERROR("{}:{} - Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw out_of_range exception.", __FILE__, __LINE__, m_name, parsedText);
				EG_CORE_ERROR("Exception Description: {}", e.what());
				return std::nullopt;
			}
		}
		else
		{
			text = std::move(s);
		}
	}

	return text;
}
std::unique_ptr<ColorBrush> TextLoader::ParseBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (!data.contains("Brush"))
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': No 'Brush' key. Incomplete Text object: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	return std::move(JSONLoaders::LoadBrush(deviceResources, data["Brush"]));
}
std::shared_ptr<TextStyle> TextLoader::ParseStyle(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// Must parse out the name of the TextStyle before you can call JSONLoaders::LoadStyle
	std::string stylename;
	if (data.contains("Style"))
	{
		if (data["Style"].is_string())
		{
			stylename = data["Style"].get<std::string>();
			if (stylename.size() == 0)
			{
				EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Style' field cannot be an empty string.", __FILE__, __LINE__, m_name);
				return nullptr;
			}
		}
		else
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Style' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["Style"].dump(4));
			return nullptr;
		}
	}
	else
	{
		stylename = m_name + "_TextStyle";
	}


	std::shared_ptr<Style> style = JSONLoaders::LoadStyle(deviceResources, "TextStyle", data, stylename);
	if (style == nullptr)
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': JSONLoaders::GetStyle() failed for Text object: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	return std::static_pointer_cast<TextStyle>(style);
}



}