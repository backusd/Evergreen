#pragma once
#include "pch.h"
#include "Evergreen/UI/Styles/Style.h"
#include "Evergreen/UI/Styles/TextStyle.h"
#include "Evergreen/UI/Utils/ColorHelper.h"
#include "Evergreen/Exceptions/JSONLoadersException.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

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

	static std::shared_ptr<Style> Load(std::shared_ptr<DeviceResources> deviceResources, const json& data, const std::string& name = "") { return Get().LoadImpl(deviceResources, data, name); }

private:
	TextStyleLoader() noexcept = default;

	static TextStyleLoader& Get() noexcept
	{
		static TextStyleLoader loader;
		return loader;
	}

	std::shared_ptr<Style> LoadImpl(std::shared_ptr<DeviceResources> deviceResources, const json& data, const std::string& name = "");

	Evergreen::FontFamily ParseFontFamily(const json& data);
	float ParseFontSize(const json& data);
	DWRITE_FONT_WEIGHT ParseFontWeight(const json& data);
	DWRITE_FONT_STYLE ParseFontStyle(const json& data);
	DWRITE_FONT_STRETCH ParseFontStretch(const json& data);
	DWRITE_TEXT_ALIGNMENT ParseTextAlignment(const json& data);
	DWRITE_PARAGRAPH_ALIGNMENT ParseParagraphAlignment(const json& data);
	DWRITE_WORD_WRAPPING ParseWordWrapping(const json& data);

	std::string m_name;
};
#pragma warning( pop )
}