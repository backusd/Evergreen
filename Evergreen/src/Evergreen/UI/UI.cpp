#include "pch.h"
#include "UI.h"

#include <fstream>
//#include <sstream>


#define UI_ERROR(fmt, ...) m_errorMessages.push_back(std::format(fmt, __VA_ARGS__))

namespace Evergreen
{
UI::UI(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<Window> window) noexcept :
	m_deviceResources(deviceResources),
	m_window(window),
	m_jsonRoot({}),
	m_rootLayout(nullptr)
{
	LoadDefaultUI();
}

void UI::LoadDefaultUI() noexcept
{
	// Load a default UI that displays a "No defined layout"



	// TEST CODE
	m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()));
	if (std::optional<Row*> row0 = m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f }))
	{
		row0.value()->BottomIsAdjustable(true);
	}
	if (std::optional<Row*> row1 = m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f }))
	{
		row1.value()->TopIsAdjustable(true);
	}

	m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f });
	m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f });


	std::shared_ptr<TextStyle> style = std::make_shared<TextStyle>(m_deviceResources);
	if (std::optional<Text*> text = m_rootLayout->AddControl<Text>(m_deviceResources, L"some test text", style))
	{
		EG_CORE_TRACE("{}", "Successfully created text");
	}
	else
	{
		EG_CORE_ERROR("{}", "Failed to create text");
	}


	/*
	if (std::optional<Layout*> sublayoutOpt = m_rootLayout->AddSubLayout({ 0, 0, 1, 1 }, "Test Layout"))
	{
		Layout* sublayout = sublayoutOpt.value();

		sublayout->AddRow({RowColumnType::STAR, 1.0f});
		sublayout->AddRow({ RowColumnType::STAR, 1.0f });
		sublayout->AddRow({ RowColumnType::STAR, 1.0f });

		if (std::optional<Column*> col1 = sublayout->AddColumn({ RowColumnType::STAR, 1.0f }))
		{
			col1.value()->RightIsAdjustable(true);
			col1.value()->MinWidth(10.0f);
		}
		if (std::optional<Column*> col2 = sublayout->AddColumn({ RowColumnType::STAR, 1.0f }))
		{
			col2.value()->LeftIsAdjustable(true);
			col2.value()->MinWidth(10.0f);
		}
	}
	*/
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

		// Set m_jsonRoot so we can access non-root keys for import within the Load methods
		m_jsonRoot = data.value();

		// Get the root layout data and create the root layout
		json rootLayoutData = m_jsonRoot["root"];

		m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()), "Root Layout");
		if (!LoadLayoutDetails(m_rootLayout.get(), rootLayoutData))
		{
			m_jsonRoot = {};
			LoadErrorUI();
			return;
		}

		// LayoutCheck is entirely optional - In a Release build, this does nothing
		m_rootLayout->LayoutCheck();
	}
	else
	{
		m_jsonRoot = {};
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

void UI::Render(DeviceResources* deviceResources) const noexcept
{
	deviceResources->BeginDraw();

	m_rootLayout->Render(deviceResources);

	deviceResources->EndDraw();
}

bool UI::LoadLayoutDetails(Layout* layout, json& data) noexcept
{
	// First, import any necessary data
	if (data.contains("import"))
	{
		if (!ImportJSON(data))
			return false;
	}

	if (!LoadLayoutRowDefinitions(layout, data))
		return false;

	if (!LoadLayoutColumnDefinitions(layout, data))
		return false;

	for (const auto& row : layout->Rows())
		EG_CORE_INFO("{}", row);
	for (const auto& column : layout->Columns())
		EG_CORE_INFO("{}", column);

	// Now iterate over the controls and sublayouts within the layout
	for (auto& [key, value] : data.items())
	{
		if (key.compare("import") == 0 ||
			key.compare("Type") == 0 ||
			key.compare("Row") == 0 ||
			key.compare("Column") == 0 ||
			key.compare("RowSpan") == 0 ||
			key.compare("ColumnSpan") == 0 ||
			key.compare("RowDefinitions") == 0 || 
			key.compare("ColumnDefinitions") == 0)
			continue;

		if (!data[key].contains("Type"))
		{
			EG_CORE_ERROR("{}:{} - Control or sub-layout has no 'Type' definition: {}", __FILE__, __LINE__, data[key]);
			UI_ERROR("Control or sub-layout has no 'Type' definition: {}", data[key]);
			return false;
		}

		if (!data[key]["Type"].is_string())
		{
			EG_CORE_ERROR("{}:{} - Control or sub-layout 'Type' definition must be a string. Invalid value: {}", __FILE__, __LINE__, data[key]["Type"]);
			UI_ERROR("{}", "Control or sub-layout 'Type' definition must be a string.");
			UI_ERROR("Invalid value : {}", data[key]["Type"]);
			return false;
		}

		std::string type = data[key]["Type"].get<std::string>();

		if (type.compare("Layout") == 0)
		{
			if (!LoadSubLayout(layout, data[key], key))
				return false;
		}
		else
		{
			EG_CORE_WARN("Attempting to load control: {} (... not yet supported ...)", type);
		}
	}

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
				RowColumnType rowColType = RowColumnType::FIXED;
				float rowColSize = 1.0f;
				if (!ParseRowColumnTypeAndSize(rowDefinition["Height"], layout, rowColType, rowColSize))
					return false;

				// Create the row and get a pointer to it so we can edit it further
				if (std::optional<Row*> row = layout->AddRow({ rowColType, rowColSize }))
				{
					// Before looping over all the row definition key/value pairs, first check if we need to import other json data
					if (rowDefinition.contains("import"))
					{
						if (!ImportJSON(rowDefinition))
							return false;
					}

					// Iterate over the other keys in the row definition
					for (auto& [key, value] : rowDefinition.items()) 
					{
						if (key.compare("Height") == 0 || key.compare("comment") == 0 || key.compare("import") == 0)
							continue;
						else if (key.compare("TopAdjustable") == 0)
						{
							if (rowDefinition[key].is_boolean())
								row.value()->TopIsAdjustable(rowDefinition[key].get<bool>());
							else
							{
								EG_CORE_ERROR("{}:{} - RowDefinition TopAdjustable field for layout '{}' must have boolean type. Invalid value: {}", __FILE__, __LINE__, layout->Name(), rowDefinition[key]);
								UI_ERROR("RowDefinition TopAdjustable field for layout '{}' must have boolean type", layout->Name());
								UI_ERROR("Invalid value: {}", rowDefinition[key]);
								return false;
							}
						}
						else if (key.compare("BottomAdjustable") == 0)
						{
							if (rowDefinition[key].is_boolean())
								row.value()->BottomIsAdjustable(rowDefinition[key].get<bool>());
							else
							{
								EG_CORE_ERROR("{}:{} - RowDefinition BottomAdjustable field for layout '{}' must have boolean type. Invalid value: {}", __FILE__, __LINE__, layout->Name(), rowDefinition[key]);
								UI_ERROR("RowDefinition BottomAdjustable field for layout '{}' must have boolean type", layout->Name());
								UI_ERROR("Invalid value: {}", rowDefinition[key]);
								return false;
							}
						}
						else if (key.compare("MaxHeight") == 0)
						{
							if (!ParseRowColumnTypeAndSize(rowDefinition[key], layout, rowColType, rowColSize))
								return false;

							row.value()->MaxHeight({ rowColType, rowColSize });
						}
						else if (key.compare("MinHeight") == 0)
						{
							if (!ParseRowColumnTypeAndSize(rowDefinition[key], layout, rowColType, rowColSize))
								return false;

							row.value()->MinHeight({ rowColType, rowColSize });
						}
						else
						{
							// Just a warning because the key will be ignored
							EG_CORE_WARN("{}:{} - Unrecognized key ({}) in RowDefinition for layout '{}'.", __FILE__, __LINE__, key, layout->Name());
						}
					}
				}
				else
				{
					EG_CORE_ERROR("{}:{} - Failed to add a row for layout with name: {}", __FILE__, __LINE__, layout->Name());
					EG_CORE_ERROR("   Intended type: {} - Intended height: {}", rowColType, rowColSize);
					UI_ERROR("Failed to add a row for layout with name: {}", layout->Name());
					UI_ERROR("Intended type: {} - Intended height: {}", rowColType, rowColSize);
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
	// The 'ColumnDefinitions' parameter is not required
	// If not included, a single column will be created that spans the layout
	if (data.contains("ColumnDefinitions"))
	{
		if (data["ColumnDefinitions"].is_array())
		{
			// Iterate the list of column definitions
			for (auto& columnDefinition : data["ColumnDefinitions"])
			{
				// Each column must contain a 'Width' key
				if (!columnDefinition.contains("Width"))
				{
					EG_CORE_ERROR("{}:{} - ColumnDefinition for layout '{}' does not contain 'Width' key: {}", __FILE__, __LINE__, layout->Name(), columnDefinition);
					UI_ERROR("ColumnDefinition for layout '{}' does not contain 'Width' key: {}", layout->Name(), columnDefinition);
					return false;
				}

				// Get the Row type and size
				RowColumnType rowColType = RowColumnType::FIXED;
				float rowColSize = 1.0f;
				if (!ParseRowColumnTypeAndSize(columnDefinition["Width"], layout, rowColType, rowColSize))
					return false;

				// Create the column and get a pointer to it so we can edit it further
				if (std::optional<Column*> column = layout->AddColumn({ rowColType, rowColSize }))
				{
					// Before looping over all the row definition key/value pairs, first check if we need to import other json data
					if (columnDefinition.contains("import"))
					{
						if (!ImportJSON(columnDefinition))
							return false;
					}

					// Iterate over the other keys in the column definition
					for (auto& [key, value] : columnDefinition.items())
					{
						if (key.compare("Width") == 0 || key.compare("comment") == 0 || key.compare("import") == 0)
							continue;
						else if (key.compare("LeftAdjustable") == 0)
						{
							if (columnDefinition[key].is_boolean())
								column.value()->LeftIsAdjustable(columnDefinition[key].get<bool>());
							else
							{
								EG_CORE_ERROR("{}:{} - ColumnDefinition LeftAdjustable field for layout '{}' must have boolean type. Invalid value: {}", __FILE__, __LINE__, layout->Name(), columnDefinition[key]);
								UI_ERROR("ColumnDefinition LeftAdjustable field for layout '{}' must have boolean type", layout->Name());
								UI_ERROR("Invalid value: {}", columnDefinition[key]);
								return false;
							}
						}
						else if (key.compare("RightAdjustable") == 0)
						{
							if (columnDefinition[key].is_boolean())
								column.value()->RightIsAdjustable(columnDefinition[key].get<bool>());
							else
							{
								EG_CORE_ERROR("{}:{} - ColumnDefinition RightAdjustable field for layout '{}' must have boolean type. Invalid value: {}", __FILE__, __LINE__, layout->Name(), columnDefinition[key]);
								UI_ERROR("ColumnDefinition RightAdjustable field for layout '{}' must have boolean type", layout->Name());
								UI_ERROR("Invalid value: {}", columnDefinition[key]);
								return false;
							}
						}
						else if (key.compare("MaxWidth") == 0)
						{
							if (!ParseRowColumnTypeAndSize(columnDefinition[key], layout, rowColType, rowColSize))
								return false;

							column.value()->MaxWidth({ rowColType, rowColSize });
						}
						else if (key.compare("MinWidth") == 0)
						{
							if (!ParseRowColumnTypeAndSize(columnDefinition[key], layout, rowColType, rowColSize))
								return false;

							column.value()->MinWidth({ rowColType, rowColSize });
						}
						else
						{
							// Just a warning because the key will be ignored
							EG_CORE_WARN("{}:{} - Unrecognized key ({}) in ColumnDefinition for layout '{}'.", __FILE__, __LINE__, key, layout->Name());
						}
					}
				}
				else
				{
					EG_CORE_ERROR("{}:{} - Failed to add a column for layout with name: {}", __FILE__, __LINE__, layout->Name());
					EG_CORE_ERROR("   Intended type: {} - Intended width: {}", rowColType, rowColSize);
					UI_ERROR("Failed to add a column for layout with name: {}", layout->Name());
					UI_ERROR("Intended type: {} - Intended width: {}", rowColType, rowColSize);
					return false;
				}
			}
		}
		else
		{
			EG_CORE_ERROR("{}:{} - ColumnDefinitions value must be an array type. Invalid value: {}", __FILE__, __LINE__, data["ColumnDefinitions"]);
			UI_ERROR("ColumnDefinitions value must be an array type. Invalid value: {}", data["ColumnDefinitions"]);
			return false;
		}
	}
	else
	{
		// Add a single column that spans the layout
		if (!layout->AddColumn({ RowColumnType::STAR, 1.0f }))
		{
			EG_CORE_ERROR("{}:{} - Failed to add a single default column for layout with name: {}", __FILE__, __LINE__, layout->Name());
			UI_ERROR("Failed to add a single default column for layout with name: {}", layout->Name());
			return false;
		}
	}

	return true;
}
bool UI::LoadSubLayout(Layout* parent, json& data, const std::string& name) noexcept
{
	unsigned int row = 0;
	unsigned int column = 0;
	unsigned int rowSpan = 1;
	unsigned int columnSpan = 1;

	// First, import any necessary data
	if (data.contains("import"))
	{
		if (!ImportJSON(data))
			return false;
	}

	if (data.contains("Row"))
	{
		if (!data["Row"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - Sub-layout 'Row' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["Row"]);
			UI_ERROR("{}", "Sub-layout 'Row' value must be an unsigned int.");
			UI_ERROR("Invalid value : {}", data["Row"]);
			return false;
		}

		row = data["Row"].get<unsigned int>();
	}

	if (data.contains("Column"))
	{
		if (!data["Column"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - Sub-layout 'Column' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["Column"]);
			UI_ERROR("{}", "Sub-layout 'Column' value must be an unsigned int.");
			UI_ERROR("Invalid value : {}", data["Column"]);
			return false;
		}

		column = data["Column"].get<unsigned int>();
	}

	if (data.contains("RowSpan"))
	{
		if (!data["RowSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - Sub-layout 'RowSpan' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["RowSpan"]);
			UI_ERROR("{}", "Sub-layout 'RowSpan' value must be an unsigned int.");
			UI_ERROR("Invalid value : {}", data["RowSpan"]);
			return false;
		}

		rowSpan = data["RowSpan"].get<unsigned int>();

		if (rowSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Sub-layout with name '{}' has 'RowSpan' value of 0. Setting rowSpan = 1", __FILE__, __LINE__, name);
			rowSpan = 1;
		}
	}

	if (data.contains("ColumnSpan"))
	{
		if (!data["ColumnSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - Sub-layout 'ColumnSpan' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["ColumnSpan"]);
			UI_ERROR("{}", "Sub-layout 'ColumnSpan' value must be an unsigned int.");
			UI_ERROR("Invalid value : {}", data["ColumnSpan"]);
			return false;
		}

		columnSpan = data["ColumnSpan"].get<unsigned int>();

		if (columnSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Sub-layout with name '{}' has 'ColumnSpan' value of 0. Setting columnSpan = 1", __FILE__, __LINE__, name);
			columnSpan = 1;
		}
	}

	if (std::optional<Layout*> sublayout = parent->AddSubLayout({ row, column, rowSpan, columnSpan }, name))
	{
		return LoadLayoutDetails(sublayout.value(), data);
	}


	return false;
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
			EG_CORE_ERROR("{}:{} - Fixed Height/Width values for layout '{}' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
			UI_ERROR("Fixed Height/Width value for layout '{}' must be greater than 0.", layout->Name());
			UI_ERROR("Invalid value: {}", size);
			return false;
		}
		return true;
	}
	else if (data.is_string())
	{
		std::string dataString = data.get<std::string>();
		if (dataString.size() == 0)
		{
			EG_CORE_ERROR("{}:{} - RowDefinition Height/Width value for layout '{}' must not be empty.", __FILE__, __LINE__, layout->Name());
			UI_ERROR("{}", "RowDefinition Height/Width value for layout '{}' must not be empty.", layout->Name());
			return false;
		}

		try {
			// Don't need to take a substring of datastring to remove '%' or '*' because stof will read all valid
			// digits and stop once it hits the end of the string or a non-numeric character
			size = std::stof(dataString);
		}
		catch (const std::invalid_argument& e)
		{
			EG_CORE_ERROR("{}:{} - Height/Width value ({}) for layout '{}' could not be parsed to a float.", __FILE__, __LINE__, dataString, layout->Name());
			EG_CORE_ERROR("    Exception message: {}", e.what());
			UI_ERROR("Layout: {}", layout->Name());
			UI_ERROR("Invalid Height/Width value could not be parsed to a float: {}", dataString);
			return false;
		}
		catch (const std::out_of_range& e)
		{
			EG_CORE_ERROR("{}:{} - Height/Width value ({}) for layout '{}' was out of range and could not be parsed to a float.", __FILE__, __LINE__, dataString, layout->Name());
			EG_CORE_ERROR("    Exception message: {}", e.what());
			UI_ERROR("Layout: {}", layout->Name());
			UI_ERROR("Invalid Height/Width value was out of range and could not be parsed to a float: {}", dataString);
			return false;
		}

		// Get the type and perform some bounds checking
		switch (dataString.back())
		{
		case '%': 
			type = RowColumnType::PERCENT; 
			if (size < 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Percent Height/Width value for layout '{}' cannot be less than 0. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				UI_ERROR("Percent Height/Width value for layout '{}' cannot be less than 0.", layout->Name());
				UI_ERROR("Invalid value: {}", size);
				return false;
			}
			else if (size > 100.0f)
			{
				EG_CORE_ERROR("{}:{} - Percent Height/Width value for layout '{}' cannot be greater than 100. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				UI_ERROR("Percent Height/Width value for layout '{}' cannot be greater than 100.", layout->Name());
				UI_ERROR("Invalid value: {}", size);
				return false;
			}

			size /= 100.0f; // Must divide by 100 to stay within the range [0, 1]
			
			break;
		case '*': 
			type = RowColumnType::STAR; 
			if (size <= 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Star Height/Width value for layout '{}' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				UI_ERROR("Star Height/Width value for layout '{}' must be greater than 0.", layout->Name());
				UI_ERROR("Invalid value: {}", size);
				return false;
			}
			break;
		default:
			type = RowColumnType::FIXED; 
			if (size <= 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Fixed Height/Width value for layout '{}' must be greater than 0. Invalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				UI_ERROR("Fixed Height/Width value for layout '{}' must be greater than 0.", layout->Name());
				UI_ERROR("Invalid value: {}", size);
				return false;
			}
			break;
		}

		return true;
	}

	EG_CORE_ERROR("{}:{} - Height/Width value must be either a number or a string. Layout: {}. Invalid value: {}", __FILE__, __LINE__, layout->Name(), data);
	UI_ERROR("{}", "Height/Width value must be either a number or a string.");
	UI_ERROR("Layout: {}", layout->Name());
	UI_ERROR("Invalid value: {}", data);
	return false;
}

bool UI::ImportJSON(json& data) noexcept
{
	if (!data.contains("import"))
	{
		EG_CORE_WARN("{}:{} - UI::ImportJSON called but could not find 'import' key: {}", __FILE__, __LINE__, data);
		return true; // Return true because this is not a true error. The program can still continue to parse the json data.
	}

	if (!data["import"].is_string())
	{
		EG_CORE_ERROR("{}:{} - 'import' key must have a string-type value. Invalid value: {}", __FILE__, __LINE__, data["import"]);
		UI_ERROR("'import' key must have a string-type value. Invalid value: {}", data["import"]);
		return false;
	}

	std::string importValue = data["import"].get<std::string>();

	if (importValue.size() == 0)
	{
		EG_CORE_WARN("{}:{} - 'import' key has an empty value. json data: {}", __FILE__, __LINE__, data);
		return true; // Return true because this is not a true error. The program can still continue to parse the json data.
	}

	json jsonImport;

	if (importValue.substr(importValue.size() - 5, 5).compare(".json") == 0)
	{
		std::filesystem::path importPath = std::filesystem::path(m_jsonRootDirectory).append(importValue);
		std::optional<json> jsonImportOpt = LoadJSONFile(importPath);
		if (!jsonImportOpt.has_value())
		{
			EG_CORE_ERROR("{}:{} - Failed to import json file: {}", __FILE__, __LINE__, importPath.string());
			UI_ERROR("Failed to import json file: {}", importPath.string());
			return false;
		}

		jsonImport = jsonImportOpt.value();
	}
	else
	{
		// 'import' value is a json key that should exist at the root level of the initial json object
		if (!m_jsonRoot.contains(importValue))
		{
			EG_CORE_ERROR("{}:{} - Unable to import key '{}'. Does not exist at the json root level.", __FILE__, __LINE__, importValue);
			UI_ERROR("Unable to import key '{}'. Does not exist at the json root level.", importValue);
			return false;
		}

		jsonImport = m_jsonRoot[importValue];
	}

	// The imported json must be a json object
	if (!jsonImport.is_object())
	{
		EG_CORE_ERROR("{}:{} - Unable to import key '{}'. Imported json value must be a json object. Invalid value: {}", __FILE__, __LINE__, importValue, jsonImport);
		UI_ERROR("Unable to import key '{}'. Imported json value must be a json object.", importValue);
		UI_ERROR("Invalid value: {}", jsonImport);
		return false;
	}

	// Before adding the imported json object to our original json data, determine if it also has an 'import' key, and if so, recursively import the data
	if (jsonImport.contains("import"))
	{
		if (!ImportJSON(jsonImport))
			return false;
	}

	// Loop over the imported json object adding each field to the original object
	for (auto& [key, value] : jsonImport.items())
	{
		if (key.compare("import") == 0)
			continue;

		// Make sure there are no key conflicts
		if (data.contains(key))
		{
			EG_CORE_ERROR("{}:{} - Unable to import key '{}'. Both the original and imported json contain the same key: {}", __FILE__, __LINE__, importValue, key);
			UI_ERROR("Unable to import key '{}'. Both the original and imported json contain the same key: {}", importValue, key);
			return false;
		}

		data[key] = jsonImport[key];
	}

	// Finally, remove the 'import' key because so the parsing code can't accidentally attempt to import it a second time
	data.erase("import");

	return true;
}

void UI::OnWindowResize(WindowResizeEvent& e) noexcept
{
	m_rootLayout->OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
}

void UI::OnMouseMove(MouseMoveEvent& e) noexcept
{
	m_rootLayout->OnMouseMove(e);
}
void UI::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	m_rootLayout->OnMouseButtonPressed(e);
}
void UI::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	m_rootLayout->OnMouseButtonReleased(e);
}


}