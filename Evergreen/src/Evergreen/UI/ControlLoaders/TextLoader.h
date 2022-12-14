#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"



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

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, const json& data, const std::string& name) noexcept { return std::move(Get().LoadImpl(deviceResources, parent, data, name)); }

private:
	TextLoader() noexcept = default;

	static TextLoader& Get() noexcept
	{
		static TextLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, const json& data, const std::string& name) noexcept;

	bool ValidateJSONData(const json& data) noexcept;

	std::optional<std::wstring> ParseText(const json& data) noexcept;
	std::unique_ptr<ColorBrush> ParseBrush(const json& data) noexcept;
	std::shared_ptr<TextStyle> ParseStyle(const json& data) noexcept;



	std::shared_ptr<DeviceResources> m_deviceResources;








};
#pragma warning( pop )


	/*
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

	static bool Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, GlobalJsonData* globalData) noexcept;

private:
	TextLoader() noexcept;

	static TextLoader& Get() noexcept
	{
		static TextLoader loader;
		return loader;
	}

	bool PreLoadValidation(Layout* parent, json& data, const std::string& name) noexcept override;
		
	bool ParseText(Text* textControl, json& data) noexcept;
	bool ParseStyle(Text* textControl, json& data, GlobalJsonData* globalData) noexcept;
	bool ParseTextStyleAttributes(Text* textControl, json& data) noexcept;

};
#pragma warning( pop )
*/
}