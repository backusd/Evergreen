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
	TextInputLoader& operator=(const TextInputLoader&) = delete;
	~TextInputLoader() noexcept override {}

	template<typename T>
	ND static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) requires (std::is_base_of_v<TextInput, T>)
	{ 
		return Get().LoadImpl<T>(deviceResources, parent, data, name, rowColumnPositionOverride); 
	}

private:
	TextInputLoader() noexcept = default;

	static TextInputLoader& Get() noexcept
	{
		static TextInputLoader loader;
		return loader;
	}

	template<typename T>
	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<TextInput, T>);

	std::wstring ParsePlaceholderText(json& data);

	std::tuple<RowColumnType, float> ParseRowColumnTypeAndSize(json& data);
	
	std::unique_ptr<ColorBrush> ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParsePlaceholderTextBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParseInputTextBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);

	std::unique_ptr<TextStyle> ParsePlaceholderTextStyle(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<TextStyle> ParseInputTextStyle(std::shared_ptr<DeviceResources> deviceResources, json& data);

	float ParseBorderWidth(json& data);

	// Methods to set attributes not included in the TextInput constructor
	void ParseVerticalBarBrush(TextInput* textInput, json& data);
	void ParseVerticalBarWidth(TextInput* textInput, json& data);

	void ParseRightSideLayout(TextInput* textInput, json& data);

};
#pragma warning( pop )

template<typename T>
Control* TextInputLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<TextInput, T>)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique

	RowColumnPosition rowCol;
	Margin margin;
	std::wstring placeholderText = L"";
	std::unique_ptr<ColorBrush> placeholderBrush = nullptr;
	std::unique_ptr<TextStyle> placeholderStyle = nullptr;
	std::unique_ptr<ColorBrush> inputTextBrush = nullptr;
	std::unique_ptr<TextStyle> inputTextStyle = nullptr;
	std::unique_ptr<ColorBrush> backgroundBrush = nullptr;
	std::unique_ptr<ColorBrush> borderBrush = nullptr;
	float borderWidth = 0.0f;

	// Parse Row/Column
	rowCol = rowColumnPositionOverride.has_value() ? rowColumnPositionOverride.value() : ParseRowColumnPosition(data);

	// Parse Margin
	margin = ParseMargin(data);

	// Parse Placeholder Text
	placeholderText = ParsePlaceholderText(data);

	// Parse BorderWidth
	borderWidth = ParseBorderWidth(data);

	// Parse Brushes
	placeholderBrush = ParsePlaceholderTextBrush(deviceResources, data);
	inputTextBrush = ParseInputTextBrush(deviceResources, data);
	backgroundBrush = ParseBackgroundBrush(deviceResources, data);
	borderBrush = ParseBorderBrush(deviceResources, data);

	// Parse Style
	placeholderStyle = ParsePlaceholderTextStyle(deviceResources, data);
	inputTextStyle = ParseInputTextStyle(deviceResources, data);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"BackgroundBrush", "BorderBrush", "BorderWidth", "PlaceholderText", "PlaceholderTextBrush", "PlaceholderTextStyle",
	"InputTextBrush", "InputTextStyle", "VerticalBarBrush", "VerticalBarWidth",	"RightSideLayoutColumnWidth", "RightSideLayout" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - TextInput control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	T* textInput = parent->CreateControl<T>(
		rowCol,
		deviceResources,
		placeholderText,
		std::move(placeholderBrush),
		std::move(placeholderStyle),
		std::move(inputTextBrush),
		std::move(inputTextStyle),
		std::move(backgroundBrush),
		std::move(borderBrush),
		borderWidth,
		margin
	);
	EG_CORE_ASSERT(textInput != nullptr, "Something went wrong, TextInput is nullptr");

	textInput->Name(name);
	textInput->ID(ParseID(data));

	ParseVerticalBarBrush(textInput, data);
	ParseVerticalBarWidth(textInput, data);

	ParseRightSideLayout(textInput, data);

	return textInput;
}

}