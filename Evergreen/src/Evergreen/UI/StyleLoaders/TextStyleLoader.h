#pragma once
#include "pch.h"
//#include "StyleLoader.h"
#include "Evergreen/UI/Styles/TextStyle.h"
#include "Evergreen/UI/ColorHelper.h"
#include "Evergreen/UI/BrushLoaders/SolidColorBrushLoader.h"
#include "Evergreen/UI/BrushLoaders/GradientBrushLoader.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API TextStyleLoader
{
public:
	TextStyleLoader(const TextStyleLoader&) = delete;
	void operator=(const TextStyleLoader&) = delete;
	~TextStyleLoader() noexcept {}

	static std::shared_ptr<Style> Load(std::shared_ptr<DeviceResources> deviceResources, const json& data, const std::string& name = "") noexcept { return Get().LoadImpl(deviceResources, data, name); }

private:
	TextStyleLoader() noexcept = default;

	static TextStyleLoader& Get() noexcept
	{
		static TextStyleLoader loader;
		return loader;
	}

	std::shared_ptr<Style> LoadImpl(std::shared_ptr<DeviceResources> deviceResources, const json& data, const std::string& name = "") noexcept;

	std::optional<Evergreen::FontFamily> ParseFontFamily(const json& data) noexcept;
	std::optional<float> ParseFontSize(const json& data) noexcept;
	std::optional<DWRITE_FONT_WEIGHT> ParseFontWeight(const json& data) noexcept;
	std::optional<DWRITE_FONT_STYLE> ParseFontStyle(const json& data) noexcept;
	std::optional<DWRITE_FONT_STRETCH> ParseFontStretch(const json& data) noexcept;
	std::optional<DWRITE_TEXT_ALIGNMENT> ParseTextAlignment(const json& data) noexcept;
	std::optional<DWRITE_PARAGRAPH_ALIGNMENT> ParseParagraphAlignment(const json& data) noexcept;
	std::optional<DWRITE_WORD_WRAPPING> ParseWordWrapping(const json& data) noexcept;

	std::string m_name;
};
#pragma warning( pop )

/*
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API TextStyleLoader : public StyleLoader
{
public:
	TextStyleLoader(const TextStyleLoader&) = delete;
	void operator=(const TextStyleLoader&) = delete;
	virtual ~TextStyleLoader() noexcept {}

	static std::shared_ptr<Style> Load(std::shared_ptr<DeviceResources> deviceResources, json& data, const std::string& name = "") noexcept;

private:
	TextStyleLoader() noexcept;

	static TextStyleLoader& Get() noexcept
	{
		static TextStyleLoader loader;
		return loader;
	}

	// bool PreLoadValidation(json& data, const std::string& name) noexcept override; <-- Can override this function

	bool ParseFontFamily(TextStyle* textStyle, json& data) noexcept;
	bool ParseFontSize(TextStyle* textStyle, json& data) noexcept;
	bool ParseFontWeight(TextStyle* textStyle, json& data) noexcept;
	bool ParseFontStyle(TextStyle* textStyle, json& data) noexcept;
	bool ParseFontStretch(TextStyle* textStyle, json& data) noexcept;
	bool ParseTextAlignment(TextStyle* textStyle, json& data) noexcept;
	bool ParseParagraphAlignment(TextStyle* textStyle, json& data) noexcept;
	bool ParseWordWrapping(TextStyle* textStyle, json& data) noexcept;
	bool ParseTrimming(TextStyle* textStyle, json& data) noexcept;
	bool ParseLocale(TextStyle* textStyle, json& data) noexcept;






};
#pragma warning( pop )
*/
}