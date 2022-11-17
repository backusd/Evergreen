#include "pch.h"
#include "UI.h"

#include <fstream>
#include <sstream>

#define UI_ERROR(fmt, ...) m_errorMessages.push_back(std::format(fmt, __VA_ARGS__))

namespace Evergreen
{
UI::UI(std::shared_ptr<Window> window) noexcept :
	m_window(window),
	m_rootLayout(nullptr)
{
	LoadDefaultUI();
}

void UI::LoadDefaultUI() noexcept
{
	// Load a default UI that displays a "No defined layout"




}

void UI::LoadUI(const std::string& fileName) noexcept
{
	m_errorMessages.clear();

	std::filesystem::path filePath = std::filesystem::path(m_jsonRootDirectory).append(fileName);

	if (std::optional<json> data = LoadJSONFile(filePath))
	{
		// Make sure data has 'root' key
		if (!data.value().contains("root"))
		{
			EG_CORE_ERROR("{}:{} - File {} has no 'root' key", __FILE__, __LINE__, filePath.string());
			UI_ERROR("File {} has no 'root' key", filePath.string());
			LoadErrorUI();
			return;
		}

		// Get the root layout data and create the root layout
		json rootLayoutData = data.value()["root"];

		m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()));
		if (!LoadLayoutDetails(m_rootLayout.get(), rootLayoutData))
		{
			LoadErrorUI();
			return;
		}



	}
	else
	{
		LoadErrorUI();
	}
}

std::optional<json> UI::LoadJSONFile(std::filesystem::path filePath) noexcept
{
	std::ifstream file;
	file.open(filePath);
	if (file.is_open())
	{
		std::string fileData;
		std::ostringstream oss;
		oss << file.rdbuf();
		fileData = oss.str();
		file.close();

		try
		{
			return json::parse(fileData);
		}
		catch (const json::parse_error& e)
		{
			EG_CORE_ERROR("{}:{} - UI::LoadJSONFile caught json::parse_error: {}", __FILE__, __LINE__, e.what());
			UI_ERROR("File: {}\njson::parse_error: {}", filePath.string(), e.what());
			return std::nullopt;
		}
	}

	EG_CORE_ERROR("{}:{} - UI::LoadJSONFile failed to open file {}", __FILE__, __LINE__, filePath.string());
	UI_ERROR("Failed to open file: {}", filePath.string());
	return std::nullopt;
}

void UI::LoadErrorUI() noexcept
{

}

void UI::Render() const noexcept
{

}

void UI::OnResize(float width, float height) noexcept
{
}

bool UI::LoadLayoutDetails(Layout* layout, json& data) noexcept
{
	if (!LoadLayoutName(layout, data))
		return false;

	if (!LoadLayoutRowDefinitions(layout, data))
		return false;

	if (!LoadLayoutColumnDefinitions(layout, data))
		return false;

	return true;
}
bool UI::LoadLayoutName(Layout* layout, json& data) noexcept
{
	// The 'Name' parameter is not required, but recommended
	if (data.contains("Name"))
	{
		if (data["Name"].is_string())
			layout->Name(data["Name"].get<std::string>());
		else
		{
			EG_CORE_ERROR("{}:{} - Layout name must be a string. Invalid value: {}", __FILE__, __LINE__, data["Name"]);
			return false;
		}
	}
	else
		EG_CORE_WARN("{}:{} - UI::LoadLayoutName found layout with no name", __FILE__, __LINE__);

	return true;
}
bool UI::LoadLayoutRowDefinitions(Layout* layout, json& data) noexcept
{
	// The 'RowDefinitions' parameter is not required
	// If not included, a single row will be created that spans the layout
	if (data.contains("RowDefinitions"))
	{
		if (data["RowDefinitions"].is_array())
		{
			// Iterate the list of row definitions
			for (auto& rowDefinition : data["RowDefinitions"])
			{
				// Each row must contain a 'Height' key
				if (!rowDefinition.contains("Height"))
				{
					EG_CORE_ERROR("{}:{} - RowDefinition for layout '{}' does not contain 'Height' key: {}", __FILE__, __LINE__, layout->Name(), rowDefinition);
					UI_ERROR("RowDefinition for layout '{}' does not contain 'Height' key: {}", layout->Name(), rowDefinition);
					return false;
				}

				// Get the Row type and size
				RowColumnType type = RowColumnType::FIXED;
				float heightValue = 1.0f;
				if (!ParseRowColumnTypeAndSize(rowDefinition["Height"], layout, type, heightValue))
					return false;

				// Create the row and get a pointer to it so we can edit it further
				if (std::optional<Row*> row = m_rootLayout->AddRow({ type, heightValue }))
				{
					EG_CORE_TRACE("{}", "Successfully added new row");

					// Iterate over the other keys in the row definition
					//for (auto& [key, value] : o.items()) {
					//	std::cout << key << " : " << value << "\n";
					//}












				}
				else
				{
					EG_CORE_ERROR("{}:{} - Failed to add a row for layout with name: {}", __FILE__, __LINE__, layout->Name());
					EG_CORE_ERROR("   Intended type: {} - Intended height: {}", type, heightValue);
					UI_ERROR("Failed to add a row for layout with name: {}", layout->Name());
					UI_ERROR("Intended type: {} - Intended height: {}", type, heightValue);
					return false;
				}
			}
		}
		else
		{
			EG_CORE_ERROR("{}:{} - RowDefinitions value must be an array type. Invalid value: {}", __FILE__, __LINE__, data["RowDefinitions"]);
			UI_ERROR("RowDefinitions value must be an array type. Invalid value: {}", data["RowDefinitions"]);
			return false;
		}
	}
	else
	{
		// Add a single row that spans the layout
		if (!layout->AddRow({ RowColumnType::STAR, 1.0f }))
		{
			EG_CORE_ERROR("{}:{} - Failed to add a single default row for layout with name: {}", __FILE__, __LINE__, layout->Name());
			UI_ERROR("Failed to add a single default row for layout with name: {}", layout->Name());
			return false;
		}

	}

	return true;
}
bool UI::LoadLayoutColumnDefinitions(Layout* layout, json& data) noexcept
{
	return true;
}

bool UI::ParseRowColumnTypeAndSize(json& data, Layout* layout, RowColumnType& type, float& size) noexcept
{
	// The data must be an int, float, or string
	// If int or float, the type is automatically FIXED
	// If string & last character is % -> type is PERCENT
	// If string & last character is * -> type is STAR
	// Else -> type is FIXED

	if (data.is_number())
	{
		type = RowColumnType::FIXED;
		size = data.get<float>();
		if (size <= 0.0f)
		{
			EG_CORE_ERROR("{}:{} - Fixed 'Height' value for layout '{}' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
			UI_ERROR("Fixed 'Height' value for layout '{}' must be greater than 0.", layout->Name());
			UI_ERROR("Invalid value: {}", size);
			return false;
		}

		EG_CORE_TRACE("Type: {} - Value {}", type, size);
		return true;
	}
	else if (data.is_string())
	{
		std::string dataString = data.get<std::string>();
		if (dataString.size() == 0)
		{
			EG_CORE_ERROR("{}:{} - RowDefinition 'Height' value for layout '{}' must not be empty.", __FILE__, __LINE__, layout->Name());
			UI_ERROR("{}", "RowDefinition 'Height' value for layout '{}' must not be empty.", layout->Name());
			return false;
		}

		try {
			// Don't need to take a substring of datastring to remove '%' or '*' because stof will read all valid
			// digits and stop once it hits the end of the string or a non-numeric character
			size = std::stof(dataString);
		}
		catch (const std::invalid_argument& e)
		{
			EG_CORE_ERROR("{}:{} - 'Height' value ({}) for layout '{}' could not be parsed to a float.", __FILE__, __LINE__, dataString, layout->Name());
			EG_CORE_ERROR("    Exception message: {}", e.what());
			UI_ERROR("Layout: {}", layout->Name());
			UI_ERROR("Invalid 'Height' value could not be parsed to a float: {}", dataString);
			return false;
		}
		catch (const std::out_of_range& e)
		{
			EG_CORE_ERROR("{}:{} - 'Height' value ({}) for layout '{}' was out of range and could not be parsed to a float.", __FILE__, __LINE__, dataString, layout->Name());
			EG_CORE_ERROR("    Exception message: {}", e.what());
			UI_ERROR("Layout: {}", layout->Name());
			UI_ERROR("Invalid 'Height' value was out of range and could not be parsed to a float: {}", dataString);
			return false;
		}

		// Get the type and perform some bounds checking
		switch (dataString.back())
		{
		case '%': 
			type = RowColumnType::PERCENT; 
			if (size < 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Percent 'Height' value for layout '{}' cannot be less than 0. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				UI_ERROR("Percent 'Height' value for layout '{}' cannot be less than 0.", layout->Name());
				UI_ERROR("Invalid value: {}", size);
				return false;
			}
			else if (size > 100.0f)
			{
				EG_CORE_ERROR("{}:{} - Percent 'Height' value for layout '{}' cannot be greater than 100. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				UI_ERROR("Percent 'Height' value for layout '{}' cannot be greater than 100.", layout->Name());
				UI_ERROR("Invalid value: {}", size);
				return false;
			}

			size /= 100.0f; // Must divide by 100 to stay within the range [0, 1]
			
			break;
		case '*': 
			type = RowColumnType::STAR; 
			if (size <= 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Star 'Height' value for layout '{}' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				UI_ERROR("Star 'Height' value for layout '{}' must be greater than 0.", layout->Name());
				UI_ERROR("Invalid value: {}", size);
				return false;
			}
			break;
		default:
			type = RowColumnType::FIXED; 
			if (size <= 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Fixed 'Height' value for layout '{}' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				UI_ERROR("Fixed 'Height' value for layout '{}' must be greater than 0.", layout->Name());
				UI_ERROR("Invalid value: {}", size);
				return false;
			}
			break;
		}

		EG_CORE_TRACE("Type: {} - Value {}", type, dataString);
		return true;
	}

	EG_CORE_ERROR("{}:{} - RowDefinition 'Height' value must be either a number or a string. Layout: {}. Invalid 'Height' value: {}", __FILE__, __LINE__, layout->Name(), data);
	UI_ERROR("{}", "RowDefinition 'Height' value must be either a number or a string.");
	UI_ERROR("Layout: {}", layout->Name());
	UI_ERROR("Invalid 'Height' value: {}", data);
	return false;
}

}