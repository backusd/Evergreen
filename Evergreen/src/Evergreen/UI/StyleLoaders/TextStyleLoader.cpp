#include "pch.h"
#include "TextStyleLoader.h"

namespace Evergreen
{

std::shared_ptr<Style> TextStyleLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, const json& data, const std::string& name) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	m_name = name;

	Evergreen::FontFamily fontFamily = Evergreen::FontFamily::Calibri;
	float fontSize = 12.0f;
	DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR;
	DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL;
	DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL;
	DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
	DWRITE_WORD_WRAPPING wordWrapping = DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP;

	// FontFamily
	if (std::optional<Evergreen::FontFamily> fontFamilyOpt = Get().ParseFontFamily(data))
		fontFamily = fontFamilyOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': ParseFontFamily() failed", __FILE__, __LINE__, name);
		return nullptr;
	}

	// Font Size
	if (std::optional<float> fontSizeOpt = Get().ParseFontSize(data))
		fontSize = fontSizeOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': ParseFontSize() failed", __FILE__, __LINE__, name);
		return nullptr;
	}

	// Font Weight
	if (std::optional<DWRITE_FONT_WEIGHT> fontWeightOpt = Get().ParseFontWeight(data))
		fontWeight = fontWeightOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': ParseFontWeight() failed", __FILE__, __LINE__, name);
		return nullptr;
	}

	// Font Style
	if (std::optional<DWRITE_FONT_STYLE> fontStyleOpt = Get().ParseFontStyle(data))
		fontStyle = fontStyleOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': ParseFontStyle() failed", __FILE__, __LINE__, name);
		return nullptr;
	}

	// Font Stretch
	if (std::optional<DWRITE_FONT_STRETCH> fontStretchOpt = Get().ParseFontStretch(data))
		fontStretch = fontStretchOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': ParseFontStretch() failed", __FILE__, __LINE__, name);
		return nullptr;
	}

	// Text Alignment
	if (std::optional<DWRITE_TEXT_ALIGNMENT> textAlignmentOpt = Get().ParseTextAlignment(data))
		textAlignment = textAlignmentOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': ParseTextAlignment() failed", __FILE__, __LINE__, name);
		return nullptr;
	}

	// Paragraph Alignment
	if (std::optional<DWRITE_PARAGRAPH_ALIGNMENT> paragraphAlignmentOpt = Get().ParseParagraphAlignment(data))
		paragraphAlignment = paragraphAlignmentOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': ParseParagraphAlignment() failed", __FILE__, __LINE__, name);
		return nullptr;
	}

	// Word Wrapping
	if (std::optional<DWRITE_WORD_WRAPPING> wordWrappingOpt = Get().ParseWordWrapping(data))
		wordWrapping = wordWrappingOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': ParseWordWrapping() failed", __FILE__, __LINE__, name);
		return nullptr;
	}

	if (data.contains("Trimming"))
		EG_CORE_WARN("{}:{} - TextStyle with name '{}': 'Trimming' field not yet supported", __FILE__, __LINE__, m_name);

	if (data.contains("Locale"))
		EG_CORE_WARN("{}:{} - TextStyle with name '{}': 'Locale' field not yet supported", __FILE__, __LINE__, m_name);

	return std::make_shared<TextStyle>(deviceResources, name, fontFamily, fontSize, fontWeight, 
		fontStyle, fontStretch,	textAlignment, paragraphAlignment, wordWrapping);
}

std::optional<Evergreen::FontFamily> TextStyleLoader::ParseFontFamily(const json& data) noexcept
{
	if (data.contains("FontFamily"))
	{
		if (!data["FontFamily"].is_string())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontFamily' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["FontFamily"].dump(4));
			return std::nullopt;
		}

		if (std::optional<const Evergreen::FontFamily> fontFamilyOpt = FontFamily::GetFontFamily(data["FontFamily"].get<std::string>()))
			return fontFamilyOpt.value();		

		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': FontFamily::GetFontFamily() failed for value: {}", __FILE__, __LINE__, m_name, data["FontFamily"].get<std::string>());
		return std::nullopt;
	}

	// Default value
	return Evergreen::FontFamily::Calibri;
}
std::optional<float> TextStyleLoader::ParseFontSize(const json& data) noexcept
{
	if (data.contains("FontSize"))
	{
		if (!data["FontSize"].is_number())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontSize' field must be a number. Invalid value: {}", __FILE__, __LINE__, m_name, data["FontSize"].dump(4));
			return std::nullopt;
		}

		float fontSize = data["FontSize"].get<float>();

		if (fontSize <= 0)
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontSize' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, m_name, fontSize);
			return std::nullopt;
		}

		return fontSize;
	}

	// Default value
	return 12.0f;
}
std::optional<DWRITE_FONT_WEIGHT> TextStyleLoader::ParseFontWeight(const json& data) noexcept
{
	if (data.contains("FontWeight"))
	{
		if (!data["FontWeight"].is_string())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontWeight' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["FontWeight"].dump(4));
			return std::nullopt;
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
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontWeight' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, m_name, fontWeightString);
			return std::nullopt;
		}

		return fontWeightMap.at(fontWeightString);
	}

	// Default value
	return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR;
}
std::optional<DWRITE_FONT_STYLE> TextStyleLoader::ParseFontStyle(const json& data) noexcept
{
	if (data.contains("FontStyle"))
	{
		if (!data["FontStyle"].is_string())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontStyle' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["FontStyle"].dump(4));
			return std::nullopt;
		}

		static const std::unordered_map<std::string, DWRITE_FONT_STYLE> fontStyleMap = {
			{ "Italic", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_ITALIC },
			{ "Normal", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL },
			{ "Oblique", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_OBLIQUE }
		};

		std::string fontStyleString = data["FontStyle"].get<std::string>();

		if (fontStyleMap.find(fontStyleString) == fontStyleMap.end())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontStyle' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, m_name, fontStyleString);
			return std::nullopt;
		}

		return fontStyleMap.at(fontStyleString);
	}

	// Default value
	return DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL;
}
std::optional<DWRITE_FONT_STRETCH> TextStyleLoader::ParseFontStretch(const json& data) noexcept
{
	if (data.contains("FontStretch"))
	{
		if (!data["FontStretch"].is_string())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontStretch' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["FontStretch"].dump(4));
			return std::nullopt;
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
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontStretch' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, m_name, fontStretchString);
			return std::nullopt;
		}

		return fontStretchMap.at(fontStretchString);
	}

	// Default value
	return DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL;
}
std::optional<DWRITE_TEXT_ALIGNMENT> TextStyleLoader::ParseTextAlignment(const json& data) noexcept
{
	if (data.contains("TextAlignment"))
	{
		if (!data["TextAlignment"].is_string())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'TextAlignment' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["TextAlignment"].dump(4));
			return std::nullopt;
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
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'TextAlignment' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, m_name, textAlignmentString);
			return std::nullopt;
		}

		return textAlignmentMap.at(textAlignmentString);
	}

	// Default value
	return DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING;
}
std::optional<DWRITE_PARAGRAPH_ALIGNMENT> TextStyleLoader::ParseParagraphAlignment(const json& data) noexcept
{
	if (data.contains("ParagraphAlignment"))
	{
		if (!data["ParagraphAlignment"].is_string())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'ParagraphAlignment' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["ParagraphAlignment"].dump(4));
			return std::nullopt;
		}

		static const std::unordered_map<std::string, DWRITE_PARAGRAPH_ALIGNMENT> paragraphAlignmentMap = {
			{ "Center", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER },
			{ "Far", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR },
			{ "Near", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR }
		};

		std::string paragraphAlignmentString = data["ParagraphAlignment"].get<std::string>();

		if (paragraphAlignmentMap.find(paragraphAlignmentString) == paragraphAlignmentMap.end())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'ParagraphAlignment' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, m_name, paragraphAlignmentString);
			return std::nullopt;
		}

		return paragraphAlignmentMap.at(paragraphAlignmentString);
	}

	// Default value
	return DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
}
std::optional<DWRITE_WORD_WRAPPING> TextStyleLoader::ParseWordWrapping(const json& data) noexcept
{
	if (data.contains("WordWrapping"))
	{
		if (!data["WordWrapping"].is_string())
		{
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'WordWrapping' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["WordWrapping"].dump(4));
			return std::nullopt;
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
			EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'WordWrapping' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, m_name, wordWrappingString);
			return std::nullopt;
		}

		return wordWrappingMap.at(wordWrappingString);
	}

	// Default value
	return DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP;
}











/*
TextStyleLoader::TextStyleLoader() noexcept
{
	m_keyLoaders["FontFamily"] = [this](Style* textStyle, json& data) -> bool { return this->ParseFontFamily(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["FontSize"] = [this](Style* textStyle, json& data) -> bool { return this->ParseFontSize(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["FontWeight"] = [this](Style* textStyle, json& data) -> bool { return this->ParseFontWeight(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["FontStyle"] = [this](Style* textStyle, json& data) -> bool { return this->ParseFontStyle(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["FontStretch"] = [this](Style* textStyle, json& data) -> bool { return this->ParseFontStretch(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["TextAlignment"] = [this](Style* textStyle, json& data) -> bool { return this->ParseTextAlignment(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["ParagraphAlignment"] = [this](Style* textStyle, json& data) -> bool { return this->ParseParagraphAlignment(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["WordWrapping"] = [this](Style* textStyle, json& data) -> bool { return this->ParseWordWrapping(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["Trimming"] = [this](Style* textStyle, json& data) -> bool { return this->ParseTrimming(static_cast<TextStyle*>(textStyle), data); };
	m_keyLoaders["Locale"] = [this](Style* textStyle, json& data) -> bool { return this->ParseLocale(static_cast<TextStyle*>(textStyle), data); };
}

std::shared_ptr<Style> TextStyleLoader::Load(std::shared_ptr<DeviceResources> deviceResources, json& data, const std::string& name) noexcept
{
	return Get().LoadImpl<TextStyle>(deviceResources, data, name);
}

bool TextStyleLoader::ParseFontFamily(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("FontFamily"), std::format("{}:{} - TextStyle with name '{}': json data must contain key 'FontFamily'", __FILE__, __LINE__, textStyle->Name()));

	if (!data["FontFamily"].is_string())
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontFamily' field must be a string. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["FontFamily"]);
		return false;
	}

	if (std::optional<const Evergreen::FontFamily> fontFamilyOpt = FontFamily::GetFontFamily(data["FontFamily"].get<std::string>()))
	{
		textStyle->FontFamily(fontFamilyOpt.value());
		return true;
	}

	EG_CORE_ERROR("{}:{} - TextStyle with name '{}': Unrecognized Font Family: {}", __FILE__, __LINE__, textStyle->Name(), data["FontFamily"]);
	return false;
}
bool TextStyleLoader::ParseFontSize(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("FontSize"), std::format("{}:{} - TextStyle with name '{}': json data must contain key 'FontSize'", __FILE__, __LINE__, textStyle->Name()));

	if (!data["FontSize"].is_number())
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontSize' field must be a number. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["FontSize"]);
		return false;
	}

	float fontSize = data["FontSize"].get<float>();

	if (fontSize <= 0)
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontSize' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), fontSize);
		return false;
	}

	textStyle->FontSize(fontSize);
	return true;
}
bool TextStyleLoader::ParseFontWeight(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("FontWeight"), std::format("{}:{} - TextStyle with name '{}': json data must contain key 'FontWeight'", __FILE__, __LINE__, textStyle->Name()));

	if (!data["FontWeight"].is_string())
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontWeight' field must be a string. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["FontWeight"]);
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
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontWeight' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["FontWeight"]);
		return false;
	}

	textStyle->FontWeight(fontWeightMap.at(fontWeightString));
	return true;
}
bool TextStyleLoader::ParseFontStyle(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("FontStyle"), std::format("{}:{} - TextStyle with name '{}': json data must contain key 'FontStyle'", __FILE__, __LINE__, textStyle->Name()));

	if (!data["FontStyle"].is_string())
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontStyle' field must be a string. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["FontStyle"]);
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
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontStyle' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["FontStyle"]);
		return false;
	}

	textStyle->FontStyle(fontStyleMap.at(fontStyleString));
	return true;
}
bool TextStyleLoader::ParseFontStretch(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("FontStretch"), std::format("{}:{} - TextStyle with name '{}': json data must contain key 'FontStretch'", __FILE__, __LINE__, textStyle->Name()));

	if (!data["FontStretch"].is_string())
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontStretch' field must be a string. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["FontStretch"]);
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
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'FontStretch' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["FontStretch"]);
		return false;
	}

	textStyle->FontStretch(fontStretchMap.at(fontStretchString));
	return true;
}
bool TextStyleLoader::ParseTextAlignment(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("TextAlignment"), std::format("{}:{} - TextStyle with name '{}': json data must contain key 'TextAlignment'", __FILE__, __LINE__, textStyle->Name()));

	if (!data["TextAlignment"].is_string())
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'TextAlignment' field must be a string. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["TextAlignment"]);
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
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'TextAlignment' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["TextAlignment"]);
		return false;
	}

	textStyle->TextAlignment(textAlignmentMap.at(textAlignmentString));
	return true;
}
bool TextStyleLoader::ParseParagraphAlignment(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("ParagraphAlignment"), std::format("{}:{} - TextStyle with name '{}': json data must contain key 'ParagraphAlignment'", __FILE__, __LINE__, textStyle->Name()));

	if (!data["ParagraphAlignment"].is_string())
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'ParagraphAlignment' field must be a string. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["ParagraphAlignment"]);
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
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'ParagraphAlignment' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["ParagraphAlignment"]);
		return false;
	}

	textStyle->ParagraphAlignment(paragraphAlignmentMap.at(paragraphAlignmentString));
	return true;
}
bool TextStyleLoader::ParseWordWrapping(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("WordWrapping"), std::format("{}:{} - TextStyle with name '{}': json data must contain key 'WordWrapping'", __FILE__, __LINE__, textStyle->Name()));

	if (!data["WordWrapping"].is_string())
	{
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'WordWrapping' field must be a string. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["WordWrapping"]);
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
		EG_CORE_ERROR("{}:{} - TextStyle with name '{}': 'WordWrapping' field was unrecognized. Invalid value: {}", __FILE__, __LINE__, textStyle->Name(), data["WordWrapping"]);
		return false;
	}

	textStyle->WordWrapping(wordWrappingMap.at(wordWrappingString));
	return true;
}
bool TextStyleLoader::ParseTrimming(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_WARN("{}:{} - TextStyle with name '{}': 'Trimming' field not yet supported", __FILE__, __LINE__, textStyle->Name());
	return true;
}
bool TextStyleLoader::ParseLocale(TextStyle* textStyle, json& data) noexcept
{
	EG_CORE_ASSERT(textStyle != nullptr, std::format("{}:{} - textStyle is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_WARN("{}:{} - TextStyle with name '{}': 'Locale' field not yet supported", __FILE__, __LINE__, textStyle->Name());
	return true;
}
*/
}