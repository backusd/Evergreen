#pragma once
#include "pch.h"
#include "ControlLoader.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
	class EVERGREEN_API TextLoader : public ControlLoader
	{
	public:
		TextLoader(const TextLoader&) = delete;
		void operator=(const TextLoader&) = delete;
		virtual ~TextLoader() noexcept {}

		static bool Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name) noexcept;

	private:
		TextLoader() noexcept;

		static TextLoader& Get() noexcept
		{
			static TextLoader loader;
			return loader;
		}

		static bool PreLoadValidation(Layout* parent, json& data, const std::string& name) noexcept;

		
		bool ParseText(Text* textControl, json& data) noexcept;
		bool ParseStyle(Text* textControl, json& data) noexcept;
		/*
		bool ParseColor(Text* textControl, json& data) noexcept;
		bool ParseFontFamily(Text* textControl, json& data) noexcept;
		bool ParseFontSize(Text* textControl, json& data) noexcept;
		bool ParseFontWeight(Text* textControl, json& data) noexcept;
		bool ParseFontStyle(Text* textControl, json& data) noexcept;
		bool ParseFontStretch(Text* textControl, json& data) noexcept;
		bool ParseTextAlignment(Text* textControl, json& data) noexcept;
		bool ParseParagraphAlignment(Text* textControl, json& data) noexcept;
		bool ParseWordWrapping(Text* textControl, json& data) noexcept;
		bool ParseTrimming(Text* textControl, json& data) noexcept;
		bool ParseLocale(Text* textControl, json& data) noexcept;
		*/


		bool ParseTextStyle(Text* textControl, json& data) noexcept;



	};
#pragma warning( pop )
}