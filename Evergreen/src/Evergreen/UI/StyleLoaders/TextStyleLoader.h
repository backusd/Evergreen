#pragma once
#include "pch.h"
#include "StyleLoader.h"
#include "Evergreen/UI/Styles/TextStyle.h"
#include "Evergreen/UI/ColorHelper.h"

namespace Evergreen
{
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

	static std::optional<std::shared_ptr<Style>> Load(std::shared_ptr<DeviceResources> deviceResources, json& data, const std::string& name = "") noexcept;

private:
	TextStyleLoader() noexcept;

	static TextStyleLoader& Get() noexcept
	{
		static TextStyleLoader loader;
		return loader;
	}

	// bool PreLoadValidation(json& data, const std::string& name) noexcept override; <-- Can override this function

	bool ParseColor(TextStyle* textStyle, json& data) noexcept;
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
}