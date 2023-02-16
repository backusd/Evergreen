#include "pch.h"
#include "TextInputLoader.h"
#include "Evergreen/UI/JSONLoading/StyleLoaders/TextStyleLoader.h"


namespace Evergreen
{
Control* TextInputLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name)
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
	rowCol = ParseRowColumnPosition(data);

	// Parse Margin
	margin = ParseMargin(data);

	// Parse Placeholder Text
	placeholderText = ParsePlaceholderText(data);

	// Parse BorderWidth
	borderWidth = ParseBorderWidth(data);

	// Parse Brushes
	placeholderBrush = ParsePlaceholderTextBrush(deviceResources, data); // Note: allowed to be nullptr when no placeholder text is specified

	inputTextBrush = ParseInputTextBrush(deviceResources, data);
	EG_CORE_ASSERT(inputTextBrush != nullptr, "Not allowed to return nullptr. Should have thrown exception");

	backgroundBrush = ParseBackgroundBrush(deviceResources, data);
	EG_CORE_ASSERT(backgroundBrush != nullptr, "Not allowed to return nullptr. Should have thrown exception");

	borderBrush = ParseBorderBrush(deviceResources, data, borderWidth); // Note: allowed to be nullptr if border width is 0

	// Parse Style
	placeholderStyle = ParsePlaceholderTextStyle(deviceResources, data); // Note: allowed to be nullptr when no placeholder text is specified

	inputTextStyle = ParseInputTextStyle(deviceResources, data);
	EG_CORE_ASSERT(inputTextStyle != nullptr, "Not allowed to return nullptr. Should have thrown exception");

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"BackgroundBrush", "BorderBrush", "BorderWidth", "PlaceholderText", "PlaceholderTextBrush", "PlaceholderTextStyle", 
	"InputTextBrush", "InputTextStyle", "VerticalBarBrush", "VerticalBarWidth", "OnMouseEntered", "OnMouseExited",
	"OnMouseMoved", "OnMouseLButtonDown", "OnMouseLButtonUp", "OnClick", "OnEnterKey", "OnInputTextChanged", 
	"RightSideLayoutColumnWidth", "RightSideLayout"};
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - TextInput control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	TextInput* textInput = parent->CreateControl<TextInput>(
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

	ParseOnMouseEnter(textInput, data);
	ParseOnMouseExited(textInput, data);
	ParseOnMouseMoved(textInput, data);
	ParseOnMouseLButtonDown(textInput, data);
	ParseOnMouseLButtonUp(textInput, data);
	ParseOnClick(textInput, data);
	ParseOnEnterKey(textInput, data);
	ParseOnInputTextChanged(textInput, data);

	ParseRightSideLayout(textInput, data);

	return textInput;
}

std::wstring TextInputLoader::ParsePlaceholderText(json& data)
{
	std::wstring text = L"";

	if (data.contains("PlaceholderText"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["PlaceholderText"].is_string(), "TextInput control with name '{}': 'PlaceholderText' field must be a string. Invalid value: {}", m_name, data["PlaceholderText"].dump(4));

		std::string parsedText = data["PlaceholderText"].get<std::string>();
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
				JSON_LOADER_EXCEPTION("TextInput control with name '{}': Parsing 'PlaceholderText' value ('{}') as hex value threw invalid_argument exception.\nException Description: {}", m_name, parsedText, e.what());
			}
			catch (const std::out_of_range& e)
			{
				JSON_LOADER_EXCEPTION("TextInput control with name '{}': Parsing 'PlaceholderText' value ('{}') as hex value threw out_of_range exception.\nException Description: {}", m_name, parsedText, e.what());
			}
		}
		else
		{
			text = std::move(s);
		}
	}

	return text;
}
float TextInputLoader::ParseBorderWidth(json& data)
{
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].is_number(), "TextInput control with name '{}': 'BorderWidth' value must be a number. Invalid TextInput object: {}", m_name, data.dump(4));

		float width = data["BorderWidth"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "TextInput control with name '{}': 'BorderWidth' value must be greater than or equal to 0. Invalid TextInput object: {}", m_name, data.dump(4));

		return width;
	}

	return 0.0f;
}
std::unique_ptr<ColorBrush> TextInputLoader::ParsePlaceholderTextBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// 'PlaceholderTextBrush' is only required if 'PlaceholderText' is present
	if (data.contains("PlaceholderText"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("PlaceholderTextBrush"), "TextInput control with name '{}': 'PlaceholderTextBrush' key is required when 'PlaceholderText' key is present. Incomplete Text object: {}", m_name, data.dump(4));

		return std::move(JSONLoaders::LoadBrush(deviceResources, data["PlaceholderTextBrush"]));
	}

	return nullptr;
}
std::unique_ptr<ColorBrush> TextInputLoader::ParseInputTextBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("InputTextBrush"), "TextInput control with name '{}': 'InputTextBrush' key is required. Incomplete Text object: {}", m_name, data.dump(4));

	return std::move(JSONLoaders::LoadBrush(deviceResources, data["InputTextBrush"]));
}
std::unique_ptr<ColorBrush> TextInputLoader::ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BackgroundBrush"), "TextInput control with name '{}': 'BackgroundBrush' key is required. Incomplete Text object: {}", m_name, data.dump(4));

	return std::move(JSONLoaders::LoadBrush(deviceResources, data["BackgroundBrush"]));
}
std::unique_ptr<ColorBrush> TextInputLoader::ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data, float borderWidth)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// 'BorderBrush' key is only required if 'BorderWidth' key is present and greater than 0
	if (borderWidth > 0.0f)
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BorderBrush"), "TextInput control with name '{}': 'BorderBrush' key is required if 'BorderWidth' is greater than 0. Incomplete Text object: {}", m_name, data.dump(4));

		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"]));
	}

	return nullptr;
}

std::unique_ptr<TextStyle> TextInputLoader::ParsePlaceholderTextStyle(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// 'PlaceholderTextStyle' key is only required if 'PlaceholderText' key is present
	if (data.contains("PlaceholderText"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("PlaceholderTextStyle"), "TextInput control with name '{}': 'PlaceholderTextStyle' field is required when 'PlaceholderText' field is used. Invalid value: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["PlaceholderTextStyle"].is_object(), "TextInput control with name '{}': 'PlaceholderTextStyle' value must be an object. Invalid value: {}", m_name, data.dump(4));
		
		std::unique_ptr<Style> style = JSONLoaders::LoadStyle(deviceResources, "TextStyle", data["PlaceholderTextStyle"], m_name + "_PlaceholderTextStyle");
		EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");

		return std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(style.release())));
	}

	return nullptr;
}
std::unique_ptr<TextStyle> TextInputLoader::ParseInputTextStyle(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// We will make 'InputTextStyle' mandatory
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("InputTextStyle"), "TextInput control with name '{}': 'InputTextStyle' field is required. Invalid value: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["InputTextStyle"].is_object(), "TextInput control with name '{}': 'InputTextStyle' value must be an object. Invalid value: {}", m_name, data.dump(4));

	std::unique_ptr<Style> style = JSONLoaders::LoadStyle(deviceResources, "TextStyle", data["InputTextStyle"], m_name + "_InputTextStyle");
	EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");

	return std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(style.release())));
}

void TextInputLoader::ParseVerticalBarBrush(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("VerticalBarBrush"))
	{
		textInput->SetVerticalBarBrush(std::move(JSONLoaders::LoadBrush(textInput->GetDeviceResources(), data["VerticalBarBrush"])));
	}
}
void TextInputLoader::ParseVerticalBarWidth(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("VerticalBarWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["VerticalBarWidth"].is_number(), "TextInput control with name '{}': 'VerticalBarWidth' value must be a number. Invalid TextInput object: {}", m_name, data.dump(4));

		float width = data["VerticalBarWidth"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(width > 0.0f, "TextInput control with name '{}': 'VerticalBarWidth' value must be greater than 0. Invalid TextInput object: {}", m_name, data.dump(4));

		textInput->SetVerticalBarWidth(width);
	}
}

void TextInputLoader::ParseOnMouseEnter(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("OnMouseEntered"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseEntered"].is_string(), "TextInput control with name '{}': 'OnMouseEntered' value must be a string. Invalid TextInput object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseEntered"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<TextInput, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "TextInput control with name '{}': 'OnMouseEnter' callback not found for key '{}'. Invalid TextInput object: {}", m_name, key, data.dump(4));
		textInput->SetOnMouseEnteredCallback(callback);
	}
}
void TextInputLoader::ParseOnMouseExited(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("OnMouseExited"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseExited"].is_string(), "TextInput control with name '{}': 'OnMouseExited' value must be a string. Invalid TextInput object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseExited"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<TextInput, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "TextInput control with name '{}': 'OnMouseExited' callback not found for key '{}'. Invalid TextInput object: {}", m_name, key, data.dump(4));
		textInput->SetOnMouseExitedCallback(callback);
	}
}
void TextInputLoader::ParseOnMouseMoved(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("OnMouseMoved"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseMoved"].is_string(), "TextInput control with name '{}': 'OnMouseMoved' value must be a string. Invalid TextInput object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseMoved"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<TextInput, MouseMoveEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "TextInput control with name '{}': 'OnMouseMoved' callback not found for key '{}'. Invalid TextInput object: {}", m_name, key, data.dump(4));
		textInput->SetOnMouseMovedCallback(callback);
	}
}
void TextInputLoader::ParseOnMouseLButtonDown(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("OnMouseLButtonDown"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLButtonDown"].is_string(), "TextInput control with name '{}': 'OnMouseLButtonDown' value must be a string. Invalid TextInput object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseLButtonDown"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<TextInput, MouseButtonPressedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "TextInput control with name '{}': 'OnMouseLButtonDown' callback not found for key '{}'. Invalid TextInput object: {}", m_name, key, data.dump(4));
		textInput->SetOnMouseLButtonDownCallback(callback);
	}
}
void TextInputLoader::ParseOnMouseLButtonUp(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("OnMouseLButtonUp"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnMouseLButtonUp"].is_string(), "TextInput control with name '{}': 'OnMouseLButtonUp' value must be a string. Invalid TextInput object: {}", m_name, data.dump(4));
		std::string key = data["OnMouseLButtonUp"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<TextInput, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "TextInput control with name '{}': 'OnMouseLButtonUp' callback not found for key '{}'. Invalid TextInput object: {}", m_name, key, data.dump(4));
		textInput->SetOnMouseLButtonUpCallback(callback);
	}
}
void TextInputLoader::ParseOnClick(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("OnClick"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnClick"].is_string(), "TextInput control with name '{}': 'OnClick' value must be a string. Invalid TextInput object: {}", m_name, data.dump(4));
		std::string key = data["OnClick"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<TextInput, MouseButtonReleasedEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "TextInput control with name '{}': 'OnClick' callback not found for key '{}'. Invalid TextInput object: {}", m_name, key, data.dump(4));
		textInput->SetOnClickCallback(callback);
	}
}
void TextInputLoader::ParseOnEnterKey(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("OnEnterKey"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnEnterKey"].is_string(), "TextInput control with name '{}': 'OnEnterKey' value must be a string. Invalid TextInput object: {}", m_name, data.dump(4));
		std::string key = data["OnEnterKey"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<TextInput, CharEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "TextInput control with name '{}': 'OnEnterKey' callback not found for key '{}'. Invalid TextInput object: {}", m_name, key, data.dump(4));
		textInput->SetOnEnterKeyCallback(callback);
	}
}
void TextInputLoader::ParseOnInputTextChanged(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("OnInputTextChanged"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnInputTextChanged"].is_string(), "TextInput control with name '{}': 'OnInputTextChanged' value must be a string. Invalid TextInput object: {}", m_name, data.dump(4));
		std::string key = data["OnInputTextChanged"].get<std::string>();

		auto callback = JSONLoaders::GetCallback<TextInput, CharEvent>(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "TextInput control with name '{}': 'OnInputTextChanged' callback not found for key '{}'. Invalid TextInput object: {}", m_name, key, data.dump(4));
		textInput->SetOnInputTextChangedCallback(callback);
	}
}

void TextInputLoader::ParseRightSideLayout(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("RightSideLayoutColumnWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RightSideLayoutColumnWidth"].is_number() || data["RightSideLayoutColumnWidth"].is_string(), "TextInput control with name '{}': 'RightSideLayoutColumnWidth' value must either be a number or a string that can be parsed by JSONLoaders::ParseRowColumnTypeAndSize(). Invalid TextInput object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("RightSideLayout"), "TextInput control with name '{}': 'RightSideLayout' value must be specified if 'RightSideLayoutColumnWidth' is specified. Invalid TextInput object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RightSideLayout"].is_object(), "TextInput control with name '{}': 'RightSideLayout' value must be a json object. Invalid TextInput object: {}", m_name, data.dump(4));

		auto [rowColType, rowColSize] = JSONLoaders::ParseRowColumnTypeAndSize(data["RightSideLayoutColumnWidth"], m_name);
		Layout* rightLayout = textInput->AddRightColumnLayout({ rowColType, rowColSize });
		EG_CORE_ASSERT(rightLayout != nullptr, "should not be able to return nullptr");

		JSONLoaders::LoadLayout(textInput->GetDeviceResources(), rightLayout, data["RightSideLayout"]);
	}
	else
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("RightSideLayout"), "TextInput control with name '{}': When 'RightSideLayout' value is specified, 'RightSideLayoutColumnWidth' is required. Invalid TextInput object: {}", m_name, data.dump(4));
	}
}
}