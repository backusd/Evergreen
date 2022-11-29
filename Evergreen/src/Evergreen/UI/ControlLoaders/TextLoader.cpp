#include "pch.h"
#include "TextLoader.h"
#include "Evergreen/UI/StyleLoaders/TextStyleLoader.h"


namespace Evergreen
{
TextLoader::TextLoader() noexcept
{
	m_keyLoaders["Text"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseText(static_cast<Text*>(textControl), data); };
	m_keyLoaders["Style"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseStyle(static_cast<Text*>(textControl), data); };

	// For TextStyle attributes, just call ParseTextStyle (will only do any actual work the first time it is called)
	m_keyLoaders["Color"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontFamily"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontSize"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontWeight"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontStyle"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontStretch"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["TextAlignment"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["ParagraphAlignment"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["WordWrapping"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["Trimming"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
	m_keyLoaders["Locale"] = [this](Control* textControl, json& data, Layout*) -> bool { return this->ParseTextStyle(static_cast<Text*>(textControl), data); };
}

bool TextLoader::Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name) noexcept
{
	if (!PreLoadValidation(parent, data, name))
		return false;

	return Get().LoadImpl<Text>(deviceResources, parent, data, name);
}

bool TextLoader::PreLoadValidation(Layout* parent, json& data, const std::string& name) noexcept
{
	// Because Text may contain TextStyle fields, make sure that if 'Style' is specified,
	// no other TextStyle fields are present
	if (data.contains("Style"))
	{
		constexpr std::array testStyleFields = {
			"Color", "FontFamily", "FontSize", "FontWeight", "FontStyle", "FontStretch", "TextAlignment",
			"ParagraphAlignment", "WordWrapping", "Trimming", "Locale"
		};

		for (auto& [key, value] : data.items())
		{
			if (std::find(testStyleFields.begin(), testStyleFields.end(), key) != testStyleFields.end())
			{
				EG_CORE_ERROR("{}:{} - Invalid json for Text control with name '{}'. Cannot include '{}' field if 'Style' field is specified.", __FILE__, __LINE__, name, key);
				return false;
			}
		}
	}

	return true;
}

bool TextLoader::ParseText(Text* textControl, json& data) noexcept
{
	if (!data["Text"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Text' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["Text"]);
		return false;
	}

	std::wstring text;
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
			EG_CORE_ERROR("{}:{} - Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw invalid_argument exception.", __FILE__, __LINE__, textControl->Name(), parsedText);
			EG_CORE_ERROR("Exception Description: {}", e.what());
			return false;
		}
		catch (const std::out_of_range& e)
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw out_of_range exception.", __FILE__, __LINE__, textControl->Name(), parsedText);
			EG_CORE_ERROR("Exception Description: {}", e.what());
			return false;
		}
	}
	else
	{
		text = std::move(s);
	}

	textControl->SetText(text);

	return true;
}
bool TextLoader::ParseStyle(Text* textControl, json& data) noexcept
{
	textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

	return true;
}
bool TextLoader::ParseTextStyle(Text* textControl, json& data) noexcept
{
	// In order to parse TextStyle attributes within a Text control json object, we are going to say 
	// that when the first TextStyle attribute is found, we will pass the json data to the TextStyleLoader 
	// which will parse all of the json data and create the TextStyle. We will then assign the TextStyle to the Text
	// control. Therefore, this is only necessary if the TextStyle of the Text control is nullptr
	if (textControl->Style() == nullptr)
	{
		if (std::optional<std::shared_ptr<TextStyle>> styleOpt = TextStyleLoader::Load(textControl->GetDeviceResources(), data, textControl->Name() + "_TextStyle"))
		{
			textControl->Style(styleOpt.value());
		}
		else
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': Call to TextStyleLoader::Load() failed", __FILE__, __LINE__, textControl->Name());
			return false;
		}
	}

	return true;
}
/*
bool TextLoader::ParseColor(Text* textControl, json& data) noexcept
{
	// In order to parse TextStyle attributes within a Text control json object, we are going to say 
	// that when the first TextStyle attribute is found, we will pass the json data to the TextStyleLoader 
	// which will parse all of the json data and create the TextStyle. We will then assign the TextStyle to the Text
	// control. Therefore, this is only necessary if the TextStyle of the Text control is nullptr
	if (textControl->Style() == nullptr)
	{
		if (std::optional<std::shared_ptr<TextStyle>> styleOpt = TextStyleLoader::Load(textControl->GetDeviceResources(), data, textControl->Name() + "_TextStyle"))
		{
			textControl->Style(styleOpt.value());
		}
		else
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': Call to TextStyleLoader::Load() failed", __FILE__, __LINE__, textControl->Name());
			return false;
		}
	}

	return true;
}
bool TextLoader::ParseFontFamily(Text* textControl, json& data) noexcept
{
	if (!data["FontFamily"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontFamily' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["FontFamily"]);
		return false;
	}

	if (std::optional<const Evergreen::FontFamily> fontFamilyOpt = FontFamily::GetFontFamily(data["FontFamily"].get<std::string>()))
	{
		if (textControl->Style() == nullptr)
			textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

		textControl->Style()->FontFamily(fontFamilyOpt.value());

		return true;
	}

	EG_CORE_ERROR("{}:{} - Text control with name '{}': Unrecognized Font Family: {}", __FILE__, __LINE__, textControl->Name(), data["FontFamily"]);
	return false;
}
bool TextLoader::ParseFontSize(Text* textControl, json& data) noexcept
{
	if (!data["FontSize"].is_number())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontSize' field must be a number. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["FontSize"]);
		return false;
	}

	float fontSize = data["FontSize"].get<float>();

	if (fontSize <= 0)
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontSize' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), fontSize);
		return false;
	}

	if (textControl->Style() == nullptr)
		textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

	textControl->Style()->FontSize(fontSize);

	return true;
}
bool TextLoader::ParseFontWeight(Text* textControl, json& data) noexcept
{
	if (!data["FontWeight"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontWeight' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["FontWeight"]);
		return false;
	}

	static const std::unordered_map<std::string, DWRITE_FONT_WEIGHT> fontWeightMap = {
		{ "Black", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BLACK },
		{ "Bold", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD },
		{ "DemiBold", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_DEMI_BOLD },
		{ "ExtraBlack", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_EXTRA_BLACK },
		{ "ExtraBold", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_EXTRA_BOLD },
		{ "ExtraLight", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_EXTRA_LIGHT },
		{ "Heavy", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_HEAVY },
		{ "Light", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_LIGHT },
		{ "Medium", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_MEDIUM },
		{ "Normal", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL },
		{ "Regular", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR },
		{ "SemiBold", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_SEMI_BOLD },
		{ "SemiLight", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_SEMI_LIGHT },
		{ "Thin", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_THIN },
		{ "UltraBlack", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_ULTRA_BLACK },
		{ "UltraBold", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_ULTRA_BOLD },
		{ "UltraLight", DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_ULTRA_LIGHT },
	};

	std::string fontWeightString = data["FontWeight"].get<std::string>();

	if (fontWeightMap.find(fontWeightString) == fontWeightMap.end())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontWeight' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["FontWeight"]);
		return false;
	}

	if (textControl->Style() == nullptr)
		textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

	textControl->Style()->FontWeight(fontWeightMap.at(fontWeightString));

	return true;
}
bool TextLoader::ParseFontStyle(Text* textControl, json& data) noexcept
{
	if (!data["FontStyle"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontStyle' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["FontStyle"]);
		return false;
	}

	static const std::unordered_map<std::string, DWRITE_FONT_STYLE> fontStyleMap = {
		{ "Italic", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_ITALIC },
		{ "Normal", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL },
		{ "Oblique", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_OBLIQUE }
	};

	std::string fontStyleString = data["FontStyle"].get<std::string>();

	if (fontStyleMap.find(fontStyleString) == fontStyleMap.end())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontStyle' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["FontStyle"]);
		return false;
	}

	if (textControl->Style() == nullptr)
		textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

	textControl->Style()->FontStyle(fontStyleMap.at(fontStyleString));

	return true;
}
bool TextLoader::ParseFontStretch(Text* textControl, json& data) noexcept
{
	if (!data["FontStretch"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontStretch' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["FontStretch"]);
		return false;
	}

	static const std::unordered_map<std::string, DWRITE_FONT_STRETCH> fontStretchMap = {
		{ "Condensed", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_CONDENSED },
		{ "Expanded", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_EXPANDED },
		{ "ExtraCondensed", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_EXTRA_CONDENSED },
		{ "ExtraExpanded", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_EXTRA_EXPANDED },
		{ "Medium", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_MEDIUM },
		{ "Normal", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL },
		{ "SemiCondensed", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_SEMI_CONDENSED },
		{ "SemiExpanded", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_SEMI_EXPANDED },
		{ "UltraCondensed", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_ULTRA_CONDENSED },
		{ "UltraExpanded", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_ULTRA_EXPANDED },
		{ "Undefined", DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_UNDEFINED }
	};

	std::string fontStretchString = data["FontStretch"].get<std::string>();

	if (fontStretchMap.find(fontStretchString) == fontStretchMap.end())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'FontStretch' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["FontStretch"]);
		return false;
	}

	if (textControl->Style() == nullptr)
		textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

	textControl->Style()->FontStretch(fontStretchMap.at(fontStretchString));

	return true;
}
bool TextLoader::ParseTextAlignment(Text* textControl, json& data) noexcept
{
	// Horizontal Alignment

	if (!data["TextAlignment"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'TextAlignment' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["TextAlignment"]);
		return false;
	}

	static const std::unordered_map<std::string, DWRITE_TEXT_ALIGNMENT> textAlignmentMap = {
		{ "Center", DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER },
		{ "Justified", DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED },
		{ "Leading", DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING },
		{ "Trailing", DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING }
	};

	std::string textAlignmentString = data["TextAlignment"].get<std::string>();

	if (textAlignmentMap.find(textAlignmentString) == textAlignmentMap.end())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'TextAlignment' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["TextAlignment"]);
		return false;
	}

	if (textControl->Style() == nullptr)
		textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

	textControl->Style()->TextAlignment(textAlignmentMap.at(textAlignmentString));

	textControl->TextChanged();

	return true;
}
bool TextLoader::ParseParagraphAlignment(Text* textControl, json& data) noexcept
{
	// Vertical Alignment

	if (!data["ParagraphAlignment"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'ParagraphAlignment' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["ParagraphAlignment"]);
		return false;
	}

	static const std::unordered_map<std::string, DWRITE_PARAGRAPH_ALIGNMENT> paragraphAlignmentMap = {
		{ "Center", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER },
		{ "Far", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR },
		{ "Near", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR }
	};

	std::string paragraphAlignmentString = data["ParagraphAlignment"].get<std::string>();

	if (paragraphAlignmentMap.find(paragraphAlignmentString) == paragraphAlignmentMap.end())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'ParagraphAlignment' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["ParagraphAlignment"]);
		return false;
	}

	if (textControl->Style() == nullptr)
		textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

	textControl->Style()->ParagraphAlignment(paragraphAlignmentMap.at(paragraphAlignmentString));

	return true;
}
bool TextLoader::ParseWordWrapping(Text* textControl, json& data) noexcept
{
	if (!data["WordWrapping"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'WordWrapping' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["WordWrapping"]);
		return false;
	}

	static const std::unordered_map<std::string, DWRITE_WORD_WRAPPING> wordWrappingMap = {
		{ "Character", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_CHARACTER },
		{ "EmergencyBreak", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_EMERGENCY_BREAK },
		{ "None", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP },
		{ "Word", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD },
		{ "Wrap", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WRAP }
	};

	std::string wordWrappingString = data["WordWrapping"].get<std::string>();

	if (wordWrappingMap.find(wordWrappingString) == wordWrappingMap.end())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'WordWrapping' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["WordWrapping"]);
		return false;
	}

	if (textControl->Style() == nullptr)
		textControl->Style(std::make_shared<TextStyle>(textControl->GetDeviceResources()));

	textControl->Style()->WordWrapping(wordWrappingMap.at(wordWrappingString));

	return true;
}
bool TextLoader::ParseTrimming(Text* textControl, json& data) noexcept
{
	EG_CORE_WARN("{}:{} - Text control with name '{}': 'Trimming' field not yet supported", __FILE__, __LINE__, textControl->Name());
	return true;
}
bool TextLoader::ParseLocale(Text* textControl, json& data) noexcept
{
	EG_CORE_WARN("{}:{} - Text control with name '{}': 'Locale' field not yet supported", __FILE__, __LINE__, textControl->Name());
	return true;
}
*/

}