#include "pch.h"
#include "TextLoader.h"
#include "Evergreen/UI/StyleLoaders/TextStyleLoader.h"


namespace Evergreen
{
Control* TextLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, const json& data, const std::string& name) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;

	std::wstring text = L"";
	RowColumnPosition rowCol;

	// Validate JSON
	if (!ValidateJSONData(data))
		return nullptr;

	// Parse Row/Column
	if (std::optional<RowColumnPosition> rowColOpt = ParseRowColumnPosition(data))
		rowCol = rowColOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseRowColumnPosition() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	// Parse Text
	if (std::optional<std::wstring> textOpt = ParseText(data))
		text = textOpt.value();
	else
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseText() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	// Parse Brush
	std::unique_ptr<ColorBrush> brush = ParseBrush(deviceResources, data);
	if (brush == nullptr)
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseBrush() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	// Parse Style
	std::shared_ptr<TextStyle> style = ParseStyle(deviceResources, data);
	if (style == nullptr)
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': ParseStyle() failed for data: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	if (std::optional<Text*> textOpt = parent->AddControl<Text>(rowCol, deviceResources, text, std::move(brush), style))
		return textOpt.value();

	return nullptr;
}

bool TextLoader::ValidateJSONData(const json& data) noexcept
{
	// Because Text may contain TextStyle fields, make sure that if 'Style' is specified,
	// no other TextStyle fields are present
	if (data.contains("Style"))
	{
		constexpr std::array testStyleFields = {
			"Color", "FontFamily", "FontSize", "FontWeight", "FontStyle", "FontStretch", "TextAlignment",
			"ParagraphAlignment", "WordWrapping", "Trimming", "Locale"
		};

		for (auto& [key, value] : data.items())
		{
			if (std::find(testStyleFields.begin(), testStyleFields.end(), key) != testStyleFields.end())
			{
				EG_CORE_ERROR("{}:{} - Invalid json for Text control with name '{}'. Cannot include '{}' field if 'Style' field is specified.", __FILE__, __LINE__, m_name, key);
				return false;
			}
		}
	}

	return true;
}

std::optional<std::wstring> TextLoader::ParseText(const json& data) noexcept
{
	std::wstring text = L"";

	if (data.contains("Text"))
	{
		if (!data["Text"].is_string())
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Text' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["Text"].dump(4));
			return std::nullopt;
		}

		std::string parsedText = data["Text"].get<std::string>();
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
				EG_CORE_ERROR("{}:{} - Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw invalid_argument exception.", __FILE__, __LINE__, m_name, parsedText);
				EG_CORE_ERROR("Exception Description: {}", e.what());
				return std::nullopt;
			}
			catch (const std::out_of_range& e)
			{
				EG_CORE_ERROR("{}:{} - Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw out_of_range exception.", __FILE__, __LINE__, m_name, parsedText);
				EG_CORE_ERROR("Exception Description: {}", e.what());
				return std::nullopt;
			}
		}
		else
		{
			text = std::move(s);
		}
	}

	return text;
}
std::unique_ptr<ColorBrush> TextLoader::ParseBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (!data.contains("Brush"))
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': No 'Brush' key. Incomplete Text object: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	return std::move(JSONLoaders::LoadBrush(deviceResources, data["Brush"]));
}
std::shared_ptr<TextStyle> TextLoader::ParseStyle(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// Must parse out the name of the TextStyle before you can call JSONLoaders::LoadStyle
	std::string stylename;
	if (data.contains("Style"))
	{
		if (data["Style"].is_string())
		{
			stylename = data["Style"].get<std::string>();
			if (stylename.size() == 0)
			{
				EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Style' field cannot be an empty string.", __FILE__, __LINE__, m_name);
				return nullptr;
			}
		}
		else
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Style' field must be a string. Invalid value: {}", __FILE__, __LINE__, m_name, data["Style"].dump(4));
			return nullptr;
		}
	}
	else
	{
		stylename = m_name + "_TextStyle";
	}


	std::shared_ptr<Style> style = JSONLoaders::LoadStyle(deviceResources, "TextStyle", data, stylename);
	if (style == nullptr)
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': JSONLoaders::GetStyle() failed for Text object: {}", __FILE__, __LINE__, m_name, data.dump(4));
		return nullptr;
	}

	return std::static_pointer_cast<TextStyle>(style);
}




/*
TextLoader::TextLoader() noexcept
{
	m_keyLoaders["Text"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseText(static_cast<Text*>(textControl), data); };
	m_keyLoaders["Style"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData* globalData) -> bool { return this->ParseStyle(static_cast<Text*>(textControl), data, globalData); };

	// For TextStyle attributes, just call ParseTextStyleAttributes (will only do any actual work the first time it is called)
	m_keyLoaders["Color"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontFamily"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontSize"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontWeight"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontStyle"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["FontStretch"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["TextAlignment"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["ParagraphAlignment"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["WordWrapping"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["Trimming"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
	m_keyLoaders["Locale"] = [this](Control* textControl, json& data, Layout*, GlobalJsonData*) -> bool { return this->ParseTextStyleAttributes(static_cast<Text*>(textControl), data); };
}

bool TextLoader::Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, GlobalJsonData* globalData) noexcept
{
	return Get().LoadImpl<Text>(deviceResources, parent, data, name, globalData);
}

bool TextLoader::PreLoadValidation(Layout* parent, json& data, const std::string& name) noexcept
{
	// Because Text may contain TextStyle fields, make sure that if 'Style' is specified,
	// no other TextStyle fields are present
	if (data.contains("Style"))
	{
		constexpr std::array testStyleFields = {
			"Color", "FontFamily", "FontSize", "FontWeight", "FontStyle", "FontStretch", "TextAlignment",
			"ParagraphAlignment", "WordWrapping", "Trimming", "Locale"
		};

		for (auto& [key, value] : data.items())
		{
			if (std::find(testStyleFields.begin(), testStyleFields.end(), key) != testStyleFields.end())
			{
				EG_CORE_ERROR("{}:{} - Invalid json for Text control with name '{}'. Cannot include '{}' field if 'Style' field is specified.", __FILE__, __LINE__, name, key);
				return false;
			}
		}
	}

	return true;
}

bool TextLoader::ParseText(Text* textControl, json& data) noexcept
{
	EG_CORE_ASSERT(textControl != nullptr, std::format("{}:{} - Text control cannot be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("Text"), std::format("{}:{} - Text control with name '{}': json data must contain key 'Text'", __FILE__, __LINE__, textControl->Name()));

	if (!data["Text"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Text' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["Text"]);
		return false;
	}

	std::wstring text;
	std::string parsedText = data["Text"].get<std::string>();
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
			EG_CORE_ERROR("{}:{} - Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw invalid_argument exception.", __FILE__, __LINE__, textControl->Name(), parsedText);
			EG_CORE_ERROR("Exception Description: {}", e.what());
			return false;
		}
		catch (const std::out_of_range& e)
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': Parsing 'Text' value ('{}') as hex value threw out_of_range exception.", __FILE__, __LINE__, textControl->Name(), parsedText);
			EG_CORE_ERROR("Exception Description: {}", e.what());
			return false;
		}
	}
	else
	{
		text = std::move(s);
	}

	textControl->SetText(text);

	return true;
}
bool TextLoader::ParseStyle(Text* textControl, json& data, GlobalJsonData* globalData) noexcept
{
	EG_CORE_ASSERT(textControl != nullptr, std::format("{}:{} - Text control cannot be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("Style"), std::format("{}:{} - Text control with name '{}': json data must contain key 'Style'", __FILE__, __LINE__, textControl->Name()));
	EG_CORE_ASSERT(globalData != nullptr, std::format("{}:{} - Text control with name '{}': GlobalJsonData cannot be nullptr", __FILE__, __LINE__, textControl->Name()));
	
	if (!data["Style"].is_string())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Style' field must be a string. Invalid value: {}", __FILE__, __LINE__, textControl->Name(), data["Style"]);
		return false;
	}

	std::string styleKey = data["Style"].get<std::string>();

	if (globalData->m_stylesMap.find(styleKey) == globalData->m_stylesMap.end())
	{
		EG_CORE_ERROR("{}:{} - Text control with name '{}': 'Style' value ('{}') could not found in the GlobalJsonData style map.", __FILE__, __LINE__, textControl->Name(), styleKey);
		return false;
	}

	textControl->Style(std::static_pointer_cast<TextStyle>(globalData->m_stylesMap.at(styleKey)));

	return true;
}
bool TextLoader::ParseTextStyleAttributes(Text* textControl, json& data) noexcept
{
	EG_CORE_ASSERT(textControl != nullptr, std::format("{}:{} - Text control cannot be nullptr", __FILE__, __LINE__));

	// In order to parse TextStyle attributes within a Text control json object, we are going to say 
	// that when the first TextStyle attribute is found, we will pass the json data to the TextStyleLoader 
	// which will parse all of the json data and create the TextStyle. We will then assign the TextStyle to the Text
	// control. Therefore, this is only necessary if the TextStyle of the Text control is nullptr
	if (textControl->Style() == nullptr)
	{
		if (std::optional<std::shared_ptr<Style>> styleOpt = TextStyleLoader::Load(textControl->GetDeviceResources(), data, textControl->Name() + "_TextStyle"))
		{
			textControl->Style(std::static_pointer_cast<TextStyle>(styleOpt.value()));
		}
		else
		{
			EG_CORE_ERROR("{}:{} - Text control with name '{}': Call to TextStyleLoader::Load() failed", __FILE__, __LINE__, textControl->Name());
			return false;
		}
	}

	return true;
}
*/
}