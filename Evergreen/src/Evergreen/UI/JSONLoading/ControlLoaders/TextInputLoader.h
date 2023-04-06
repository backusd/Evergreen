#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/TextInput.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API TextInputLoader : public ControlLoader
{
public:
	TextInputLoader(const TextInputLoader&) = delete;
	void operator=(const TextInputLoader&) = delete;
	~TextInputLoader() noexcept override {}

	static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) { return Get().LoadImpl(deviceResources, parent, data, name, rowColumnPositionOverride); }

private:
	TextInputLoader() noexcept = default;

	static TextInputLoader& Get() noexcept
	{
		static TextInputLoader loader;
		return loader;
	}

	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride);

	std::wstring ParsePlaceholderText(json& data);
	
	std::unique_ptr<ColorBrush> ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data, float borderWidth);
	std::unique_ptr<ColorBrush> ParsePlaceholderTextBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParseInputTextBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);

	std::unique_ptr<TextStyle> ParsePlaceholderTextStyle(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<TextStyle> ParseInputTextStyle(std::shared_ptr<DeviceResources> deviceResources, json& data);

	float ParseBorderWidth(json& data);

	// Methods to set attributes not included in the TextInput constructor
	void ParseVerticalBarBrush(TextInput* textInput, json& data);
	void ParseVerticalBarWidth(TextInput* textInput, json& data);

	void ParseOnMouseEnter(TextInput* textInput, json& data);
	void ParseOnMouseExited(TextInput* textInput, json& data);
	void ParseOnMouseMoved(TextInput* textInput, json& data);
	void ParseOnMouseLButtonDown(TextInput* textInput, json& data);
	void ParseOnMouseLButtonUp(TextInput* textInput, json& data);
	void ParseOnClick(TextInput* textInput, json& data);
	void ParseOnEnterKey(TextInput* textInput, json& data);
	void ParseOnInputTextChanged(TextInput* textInput, json& data);

	void ParseRightSideLayout(TextInput* textInput, json& data);

};
#pragma warning( pop )

}