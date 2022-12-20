#include "pch.h"
#include "TextStyleLoader.h"

namespace Evergreen
{

std::shared_ptr<Style> TextStyleLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, const json& data, const std::string& name)
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
	fontFamily = ParseFontFamily(data);

	// Font Size
	fontSize = ParseFontSize(data);

	// Font Weight
	fontWeight = ParseFontWeight(data);

	// Font Style
	fontStyle = ParseFontStyle(data);

	// Font Stretch
	fontStretch = ParseFontStretch(data);

	// Text Alignment
	textAlignment = ParseTextAlignment(data);

	// Paragraph Alignment
	paragraphAlignment = ParseParagraphAlignment(data);

	// Word Wrapping
	wordWrapping = ParseWordWrapping(data);

	if (data.contains("Trimming"))
		EG_CORE_WARN("{}:{} - TextStyle with name '{}': 'Trimming' field not yet supported", __FILE__, __LINE__, m_name);

	if (data.contains("Locale"))
		EG_CORE_WARN("{}:{} - TextStyle with name '{}': 'Locale' field not yet supported", __FILE__, __LINE__, m_name);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "Type", "Text", "Row", "Column", "RowSpan", "ColumnSpan",
	"Style", "Brush", "FontFamily", "FontSize", "FontWeight", "FontStyle", "FontStretch", "TextAlignment",
	"ParagraphAlignment", "WordWrapping", "Trimming", "Locale" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - TextStyle with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Return a new TextStyle
	return std::make_shared<TextStyle>(deviceResources, name, fontFamily, fontSize, fontWeight, 
		fontStyle, fontStretch,	textAlignment, paragraphAlignment, wordWrapping);
}

Evergreen::FontFamily TextStyleLoader::ParseFontFamily(const json& data)
{
	if (data.contains("FontFamily"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["FontFamily"].is_string(), "TextStyle with name '{}': 'FontFamily' field must be a string. Invalid value: {}", m_name, data["FontFamily"].dump(4));

		return FontFamily::GetFontFamily(data["FontFamily"].get<std::string>());
	}

	// Default value
	return Evergreen::FontFamily::Calibri;
}
float TextStyleLoader::ParseFontSize(const json& data)
{
	if (data.contains("FontSize"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["FontSize"].is_number(), "TextStyle with name '{}': 'FontSize' field must be a number. Invalid value: {}", m_name, data["FontSize"].dump(4));

		float fontSize = data["FontSize"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(fontSize > 0.0f, "TextStyle with name '{}': 'FontSize' must be greater than 0. Invalid value: {}", m_name, fontSize);

		return fontSize;
	}

	// Default value
	return 12.0f;
}
DWRITE_FONT_WEIGHT TextStyleLoader::ParseFontWeight(const json& data)
{
	if (data.contains("FontWeight"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["FontWeight"].is_string(), "TextStyle with name '{}': 'FontWeight' field must be a string. Invalid value: {}", m_name, data["FontWeight"].dump(4));

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

		JSON_LOADER_EXCEPTION_IF_FALSE(fontWeightMap.find(fontWeightString) != fontWeightMap.end(), "TextStyle with name '{}': 'FontWeight' field was unrecognized. Invalid value: {}", m_name, fontWeightString);

		return fontWeightMap.at(fontWeightString);
	}

	// Default value
	return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR;
}
DWRITE_FONT_STYLE TextStyleLoader::ParseFontStyle(const json& data)
{
	if (data.contains("FontStyle"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["FontStyle"].is_string(), "TextStyle with name '{}': 'FontStyle' field must be a string. Invalid value: {}", m_name, data["FontStyle"].dump(4));

		static const std::unordered_map<std::string, DWRITE_FONT_STYLE> fontStyleMap = {
			{ "Italic", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_ITALIC },
			{ "Normal", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL },
			{ "Oblique", DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_OBLIQUE }
		};

		std::string fontStyleString = data["FontStyle"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(fontStyleMap.find(fontStyleString) != fontStyleMap.end(), "TextStyle with name '{}': 'FontStyle' field was unrecognized. Invalid value: {}", m_name, fontStyleString);

		return fontStyleMap.at(fontStyleString);
	}

	// Default value
	return DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL;
}
DWRITE_FONT_STRETCH TextStyleLoader::ParseFontStretch(const json& data)
{
	if (data.contains("FontStretch"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["FontStretch"].is_string(), "TextStyle with name '{}': 'FontStretch' field must be a string. Invalid value: {}", m_name, data["FontStretch"].dump(4));

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

		JSON_LOADER_EXCEPTION_IF_FALSE(fontStretchMap.find(fontStretchString) != fontStretchMap.end(), "TextStyle with name '{}': 'FontStretch' field was unrecognized. Invalid value: {}", m_name, fontStretchString);

		return fontStretchMap.at(fontStretchString);
	}

	// Default value
	return DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL;
}
DWRITE_TEXT_ALIGNMENT TextStyleLoader::ParseTextAlignment(const json& data)
{
	if (data.contains("TextAlignment"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["TextAlignment"].is_string(), "TextStyle with name '{}': 'TextAlignment' field must be a string. Invalid value: {}", m_name, data["TextAlignment"].dump(4));

		static const std::unordered_map<std::string, DWRITE_TEXT_ALIGNMENT> textAlignmentMap = {
			{ "Center", DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER },
			{ "Justified", DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED },
			{ "Leading", DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING },
			{ "Trailing", DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING }
		};

		std::string textAlignmentString = data["TextAlignment"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(textAlignmentMap.find(textAlignmentString) != textAlignmentMap.end(), "TextStyle with name '{}': 'TextAlignment' field was unrecognized. Invalid value: {}", m_name, textAlignmentString);

		return textAlignmentMap.at(textAlignmentString);
	}

	// Default value
	return DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING;
}
DWRITE_PARAGRAPH_ALIGNMENT TextStyleLoader::ParseParagraphAlignment(const json& data)
{
	if (data.contains("ParagraphAlignment"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ParagraphAlignment"].is_string(), "TextStyle with name '{}': 'ParagraphAlignment' field must be a string. Invalid value: {}", m_name, data["ParagraphAlignment"].dump(4));

		static const std::unordered_map<std::string, DWRITE_PARAGRAPH_ALIGNMENT> paragraphAlignmentMap = {
			{ "Center", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER },
			{ "Far", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR },
			{ "Near", DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR }
		};

		std::string paragraphAlignmentString = data["ParagraphAlignment"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(paragraphAlignmentMap.find(paragraphAlignmentString) != paragraphAlignmentMap.end(), "TextStyle with name '{}': 'ParagraphAlignment' field was unrecognized. Invalid value: {}", m_name, paragraphAlignmentString);

		return paragraphAlignmentMap.at(paragraphAlignmentString);
	}

	// Default value
	return DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
}
DWRITE_WORD_WRAPPING TextStyleLoader::ParseWordWrapping(const json& data)
{
	if (data.contains("WordWrapping"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["WordWrapping"].is_string(), "TextStyle with name '{}': 'WordWrapping' field must be a string. Invalid value: {}", m_name, data["WordWrapping"].dump(4));

		static const std::unordered_map<std::string, DWRITE_WORD_WRAPPING> wordWrappingMap = {
			{ "Character", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_CHARACTER },
			{ "EmergencyBreak", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_EMERGENCY_BREAK },
			{ "None", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP },
			{ "Word", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD },
			{ "Wrap", DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WRAP }
		};

		std::string wordWrappingString = data["WordWrapping"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(wordWrappingMap.find(wordWrappingString) != wordWrappingMap.end(), "TextStyle with name '{}': 'WordWrapping' field was unrecognized. Invalid value: {}", m_name, wordWrappingString);

		return wordWrappingMap.at(wordWrappingString);
	}

	// Default value
	return DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP;
}





}