#include "pch.h"
#include "TextInputLoader.h"
#include "Evergreen/UI/JSONLoading/StyleLoaders/TextStyleLoader.h"


namespace Evergreen
{
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
	return data.contains("PlaceholderTextBrush") ? std::move(JSONLoaders::LoadBrush(deviceResources, data["PlaceholderTextBrush"])) : nullptr;
}
std::unique_ptr<ColorBrush> TextInputLoader::ParseInputTextBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	return data.contains("InputTextBrush") ? std::move(JSONLoaders::LoadBrush(deviceResources, data["InputTextBrush"])) : nullptr;
}
std::unique_ptr<ColorBrush> TextInputLoader::ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	return data.contains("BackgroundBrush") ? std::move(JSONLoaders::LoadBrush(deviceResources, data["BackgroundBrush"])) : nullptr;
}
std::unique_ptr<ColorBrush> TextInputLoader::ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	return data.contains("BorderBrush") ? std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"])) : nullptr;
}

std::unique_ptr<TextStyle> TextInputLoader::ParsePlaceholderTextStyle(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (data.contains("PlaceholderTextStyle"))
	{
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

	if (data.contains("InputTextStyle"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["InputTextStyle"].is_object(), "TextInput control with name '{}': 'InputTextStyle' value must be an object. Invalid value: {}", m_name, data.dump(4));

		std::unique_ptr<Style> style = JSONLoaders::LoadStyle(deviceResources, "TextStyle", data["InputTextStyle"], m_name + "_InputTextStyle");
		EG_CORE_ASSERT(style != nullptr, "Not allowed to return nullptr. Should have thrown exception");

		return std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(style.release())));
	}

	return nullptr;
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

void TextInputLoader::ParseRightSideLayout(TextInput* textInput, json& data)
{
	EG_CORE_ASSERT(textInput != nullptr, "textInput should not be nullptr");

	if (data.contains("RightSideLayoutColumnWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RightSideLayoutColumnWidth"].is_number() || data["RightSideLayoutColumnWidth"].is_string(), "TextInput control with name '{}': 'RightSideLayoutColumnWidth' value must either be a number or a string that can be parsed by JSONLoaders::ParseRowColumnTypeAndSize(). Invalid TextInput object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("RightSideLayout"), "TextInput control with name '{}': 'RightSideLayout' value must be specified if 'RightSideLayoutColumnWidth' is specified. Invalid TextInput object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RightSideLayout"].is_object(), "TextInput control with name '{}': 'RightSideLayout' value must be a json object. Invalid TextInput object: {}", m_name, data.dump(4));

		auto [rowColType, rowColSize] = ParseRowColumnTypeAndSize(data["RightSideLayoutColumnWidth"]);
		Layout* rightLayout = textInput->AddRightColumnLayout({ rowColType, rowColSize });
		EG_CORE_ASSERT(rightLayout != nullptr, "should not be able to return nullptr");

		JSONLoaders::LoadLayout(textInput->GetDeviceResources(), "Layout", rightLayout, data["RightSideLayout"], std::format("{}_RightSideLayout", m_name));
	}
	else
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("RightSideLayout"), "TextInput control with name '{}': When 'RightSideLayout' value is specified, 'RightSideLayoutColumnWidth' is required. Invalid TextInput object: {}", m_name, data.dump(4));
	}
}

std::tuple<RowColumnType, float> TextInputLoader::ParseRowColumnTypeAndSize(json& data)
{
	// The data must be an int, float, or string
	// If int or float, the type is automatically FIXED
	// If string & last character is % -> type is PERCENT
	// If string & last character is * -> type is STAR
	// Else -> type is FIXED

	RowColumnType type = RowColumnType::FIXED;
	float size = 1.0f;

	if (data.is_number())
	{
		type = RowColumnType::FIXED;
		size = data.get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Fixed Height/Width value for TextInputLoader with name '{}' must be greater than 0.\nInvalid value: {}", m_name, size);

		return std::make_tuple(type, size);
	}
	else if (data.is_string())
	{
		std::string dataString = data.get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(dataString.size() > 0, "Height/Width value for TextInputLoader with name '{}' must not be empty.", m_name);

		try {
			// Don't need to take a substring of datastring to remove '%' or '*' because stof will read all valid
			// digits and stop once it hits the end of the string or a non-numeric character
			size = std::stof(dataString);
		}
		catch (const std::invalid_argument& e)
		{
			JSON_LOADER_EXCEPTION("Invalid Height/Width value for TextInputLoader with name ({}) could not be parsed to a float: {}.\nException Message: {}", m_name, dataString, e.what());
		}
		catch (const std::out_of_range& e)
		{
			JSON_LOADER_EXCEPTION("Invalid Height/Width value for TextInputLoader with name ({}). Caught out of range exception for value '{}'.\nException Message: {}", m_name, dataString, e.what());
		}

		// Get the type and perform some bounds checking
		switch (dataString.back())
		{
		case '%':
			type = RowColumnType::PERCENT;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f && size <= 100.0f, "Percent Height/Width value for TextInputLoader with name '{}' must be in the range (0, 100].\nInvalid value: {}", m_name, size);

			size /= 100.0f; // Must divide by 100 to stay within the range [0, 1]

			break;
		case '*':
			type = RowColumnType::STAR;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Star Height/Width value for TextInputLoader with name '{}' must be greater than 0.\nInvalid value: {}", m_name, size);

			break;
		default:
			type = RowColumnType::FIXED;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Fixed Height/Width value for TextInputLoader with name '{}' must be greater than 0.\nInvalid value: {}", m_name, size);

			break;
		}

		return std::make_tuple(type, size);
	}

	JSON_LOADER_EXCEPTION("Height/Width value must be either a number or a string.\nTextInputLoader name: {}\nInvalid value: {}", m_name, data.dump(4));
}

}