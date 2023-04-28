#include "pch.h"
#include "LayoutLoader.h"


namespace Evergreen
{
void LayoutLoader::LoadLayoutBrush(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "Layout cannot be nullptr");
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (data.contains("Brush"))
	{
		layout->BackgroundBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["Brush"])));
	}
}
void LayoutLoader::LoadLayoutRowDefinitions(Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "Layout cannot be nullptr");

	// The 'RowDefinitions' parameter is not required
	// If not included, a single row will be created that spans the layout
	if (data.contains("RowDefinitions"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RowDefinitions"].is_array(), "RowDefinitions value must be an array type. Invalid value: {}", data["RowDefinitions"].dump(4));

		// Iterate the list of row definitions
		for (auto& rowDefinition : data["RowDefinitions"])
		{
			// Each row must contain a 'Height' key
			JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition.contains("Height"), "RowDefinition for layout '{}' does not contain 'Height' key: {}", layout->Name(), rowDefinition.dump(4));

			// Get the Row type and size
			auto [rowColType, rowColSize] = ParseRowColumnTypeAndSize(rowDefinition["Height"], layout->Name());

			// Create the row and get a pointer to it so we can edit it further
			Row* row = layout->AddRow({ rowColType, rowColSize });

			// Before looping over all the row definition key/value pairs, first check if we need to import other json data
			JSONLoaders::ImportJSON(rowDefinition);

			// Iterate over the other keys in the row definition
			for (auto& [key, value] : rowDefinition.items())
			{
				if (key.compare("Height") == 0 || key.compare("import") == 0)
					continue;
				else if (key.compare("TopAdjustable") == 0)
				{
					JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition[key].is_boolean(), "RowDefinition TopAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), rowDefinition[key].dump(4));
					row->TopIsAdjustable(rowDefinition[key].get<bool>());
				}
				else if (key.compare("BottomAdjustable") == 0)
				{
					JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition[key].is_boolean(), "RowDefinition BottomAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), rowDefinition[key].dump(4));
					row->BottomIsAdjustable(rowDefinition[key].get<bool>());
				}
				else if (key.compare("MaxHeight") == 0)
				{
					auto [type, size] = ParseRowColumnTypeAndSize(rowDefinition[key], layout->Name());
					row->MaxHeight({ type, size });
				}
				else if (key.compare("MinHeight") == 0)
				{
					auto [type, size] = ParseRowColumnTypeAndSize(rowDefinition[key], layout->Name());
					row->MinHeight({ type, size });
				}
				else
				{
					// Just a warning because the key will be ignored
					EG_CORE_WARN("{}:{} - Unrecognized key ({}) in RowDefinition for layout '{}'.", __FILE__, __LINE__, key, layout->Name());
				}
			}
		}
	}
	else
	{
		// Add a single row that spans the layout
		layout->AddRow({ RowColumnType::STAR, 1.0f });
	}
}
void LayoutLoader::LoadLayoutColumnDefinitions(Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "Layout cannot be nullptr");

	// The 'ColumnDefinitions' parameter is not required
	// If not included, a single column will be created that spans the layout
	if (data.contains("ColumnDefinitions"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ColumnDefinitions"].is_array(), "ColumnDefinitions value must be an array type. Invalid value: {}", data["ColumnDefinitions"].dump(4));

		// Iterate the list of column definitions
		for (auto& columnDefinition : data["ColumnDefinitions"])
		{
			// Each column must contain a 'Width' key
			JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition.contains("Width"), "ColumnDefinition for layout '{}' does not contain 'Width' key: {}", layout->Name(), columnDefinition.dump(4));

			// Get the Row type and size
			auto [rowColType, rowColSize] = ParseRowColumnTypeAndSize(columnDefinition["Width"], layout->Name());

			// Create the column and get a pointer to it so we can edit it further
			Column* column = layout->AddColumn({ rowColType, rowColSize });

			// Before looping over all the row definition key/value pairs, first check if we need to import other json data
			JSONLoaders::ImportJSON(columnDefinition);

			// Iterate over the other keys in the column definition
			for (auto& [key, value] : columnDefinition.items())
			{
				if (key.compare("Width") == 0 || key.compare("import") == 0)
					continue;
				else if (key.compare("LeftAdjustable") == 0)
				{
					JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition[key].is_boolean(), "ColumnDefinition LeftAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), columnDefinition[key].dump(4));
					column->LeftIsAdjustable(columnDefinition[key].get<bool>());
				}
				else if (key.compare("RightAdjustable") == 0)
				{
					JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition[key].is_boolean(), "ColumnDefinition RightAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), columnDefinition[key].dump(4));
					column->RightIsAdjustable(columnDefinition[key].get<bool>());
				}
				else if (key.compare("MaxWidth") == 0)
				{
					auto [type, size] = ParseRowColumnTypeAndSize(columnDefinition[key], layout->Name());
					column->MaxWidth({ type, size });
				}
				else if (key.compare("MinWidth") == 0)
				{
					auto [type, size] = ParseRowColumnTypeAndSize(columnDefinition[key], layout->Name());
					column->MinWidth({ type, size });
				}
				else
				{
					// Just a warning because the key will be ignored
					EG_CORE_WARN("{}:{} - Unrecognized key ({}) in ColumnDefinition for layout '{}'.", __FILE__, __LINE__, key, layout->Name());
				}
			}
		}
	}
	else
	{
		// Add a single column that spans the layout
		layout->AddColumn({ RowColumnType::STAR, 1.0f });
	}
}
void LayoutLoader::LoadLayoutMargin(Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "Layout cannot be nullptr");

	if (data.contains("Margin"))
	{
		Margin margin{ 0 };
		json& marginData = data["Margin"];

		JSON_LOADER_EXCEPTION_IF_FALSE(marginData.is_array(), "Layout with name: '{}'. 'Margin' value must be an array. Invalid data: {}", layout->Name(), data.dump(4));

		for (auto& marginValue : marginData)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(marginValue.is_number(), "Layout with name: '{}'. 'Margin' array values must be numbers. Invalid data: {}", layout->Name(), data.dump(4));
		}

		if (marginData.size() == 1)
		{
			margin.Bottom = marginData[0].get<float>();
			margin.Left = margin.Bottom;
			margin.Right = margin.Bottom;
			margin.Top = margin.Bottom;
		}
		else if (marginData.size() == 2)
		{
			margin.Left = marginData[0].get<float>();
			margin.Right = margin.Left;

			margin.Top = marginData[1].get<float>();
			margin.Bottom = margin.Top;
		}
		else if (marginData.size() == 4)
		{
			margin.Left = marginData[0].get<float>();
			margin.Top = marginData[1].get<float>();
			margin.Right = marginData[2].get<float>();
			margin.Bottom = marginData[3].get<float>();
		}
		else
		{
			JSON_LOADER_EXCEPTION("Layout with name: '{}'. 'Margin' array must have 1, 2, or 4 values. Invalid data: {}", layout->Name(), data.dump(4));
		}

		layout->Margin(margin);
	}
}
void LayoutLoader::LoadLayoutBorder(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(layout != nullptr, "No layout");

	// Border Brush -----------------------------------------------------------------------------------
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BorderBrush"), "Layout with name '{}': When 'BorderWidth' key is present, the 'BorderBrush' key is required. Incomplete Layout object: {}", layout->Name(), data.dump(4));
	}
	if (data.contains("BorderBrush"))
	{
		layout->BorderBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"])));
	}

	// Border Width -----------------------------------------------------------------------------------
	std::array<float, 4> borderWidths{ 0.0f, 0.0f, 0.0f, 0.0f };
	if (data.contains("BorderWidth"))
	{
		if (data["BorderWidth"].is_number())
		{
			float width = data["BorderWidth"].get<float>();
			JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "Layout with name '{}': 'BorderWidth' is not allowed to be less than 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
			borderWidths.fill(width);
			layout->BorderWidth(borderWidths);
		}
		else if (data["BorderWidth"].is_array())
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].size() == 4, "Layout with name '{}': When specificying 'BorderWidth' as an array of floats, there must be exactly 4 values in the array. Invalid Layout object: {}", layout->Name(), data.dump(4));

			float width = 0.0f;
			for (unsigned int iii = 0; iii < 4; ++iii)
			{
				JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"][iii].is_number(), "Layout with name '{}': When specificying 'BorderWidth' as an array, the array values must be parsable numbers. Invalid Layout object: {}", layout->Name(), data.dump(4));
				width = data["BorderWidth"][iii].get<float>();
				JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "Layout with name '{}': 'BorderWidth' array values are not allowed to be less than 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
				borderWidths[iii] = width;
			}
			layout->BorderWidth(borderWidths);
		}
		else
		{
			JSON_LOADER_EXCEPTION("Layout with name '{}': 'BorderWidth' value must either be a number of an array of 4 floats. Invalid Layout object: {}", layout->Name(), data.dump(4));
		}
	}

	// Border Offsets -----------------------------------------------------------------------------------
	if (data.contains("BorderTopLeftOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopLeftOffsetX"].is_number(), "Layout with name '{}': 'BorderTopLeftOffsetX' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderTopLeftOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderTopLeftOffsetX' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderTopLeftOffsetX(value);
	}
	if (data.contains("BorderTopLeftOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopLeftOffsetY"].is_number(), "Layout with name '{}': 'BorderTopLeftOffsetY' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderTopLeftOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderTopLeftOffsetY' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderTopLeftOffsetY(value);
	}
	if (data.contains("BorderTopRightOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopRightOffsetX"].is_number(), "Layout with name '{}': 'BorderTopRightOffsetX' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderTopRightOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderTopRightOffsetX' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderTopRightOffsetX(value);
	}
	if (data.contains("BorderTopRightOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopRightOffsetY"].is_number(), "Layout with name '{}': 'BorderTopRightOffsetY' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderTopRightOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderTopRightOffsetY' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderTopRightOffsetY(value);
	}
	if (data.contains("BorderBottomLeftOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomLeftOffsetX"].is_number(), "Layout with name '{}': 'BorderBottomLeftOffsetX' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderBottomLeftOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderBottomLeftOffsetX' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderBottomLeftOffsetX(value);
	}
	if (data.contains("BorderBottomLeftOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomLeftOffsetY"].is_number(), "Layout with name '{}': 'BorderBottomLeftOffsetY' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderBottomLeftOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderBottomLeftOffsetY' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderBottomLeftOffsetY(value);
	}
	if (data.contains("BorderBottomRightOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomRightOffsetX"].is_number(), "Layout with name '{}': 'BorderBottomRightOffsetX' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderBottomRightOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderBottomRightOffsetX' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderBottomRightOffsetX(value);
	}
	if (data.contains("BorderBottomRightOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomRightOffsetY"].is_number(), "Layout with name '{}': 'BorderBottomRightOffsetY' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderBottomRightOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderBottomRightOffsetY' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderBottomRightOffsetY(value);
	}
}
void LayoutLoader::LoadLayoutID(Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "No layout");

	if (data.contains("id"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["id"].is_number_unsigned(), "Layout with name: '{}'. 'id' value must be an unsigned int. Invalid data: {}", layout->Name(), data.dump(4));
		layout->ID(data["id"].get<unsigned int>());
	}
}


std::tuple<RowColumnType, float> LayoutLoader::ParseRowColumnTypeAndSize(json& data, const std::string& layoutName)
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

		JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Fixed Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layoutName, size);

		return std::make_tuple(type, size);
	}
	else if (data.is_string())
	{
		std::string dataString = data.get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(dataString.size() > 0, "Height/Width value for layout '{}' must not be empty.", layoutName);

		try {
			// Don't need to take a substring of datastring to remove '%' or '*' because stof will read all valid
			// digits and stop once it hits the end of the string or a non-numeric character
			size = std::stof(dataString);
		}
		catch (const std::invalid_argument& e)
		{
			JSON_LOADER_EXCEPTION("Invalid Height/Width value for layout ({}) could not be parsed to a float: {}.\nException Message: {}", layoutName, dataString, e.what());
		}
		catch (const std::out_of_range& e)
		{
			JSON_LOADER_EXCEPTION("Invalid Height/Width value for layout ({}). Caught out of range exception for value '{}'.\nException Message: {}", layoutName, dataString, e.what());
		}

		// Get the type and perform some bounds checking
		switch (dataString.back())
		{
		case '%':
			type = RowColumnType::PERCENT;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f && size <= 100.0f, "Percent Height/Width value for layout '{}' must be in the range (0, 100].\nInvalid value: {}", layoutName, size);

			size /= 100.0f; // Must divide by 100 to stay within the range [0, 1]

			break;
		case '*':
			type = RowColumnType::STAR;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Star Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layoutName, size);

			break;
		default:
			type = RowColumnType::FIXED;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Fixed Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layoutName, size);

			break;
		}

		return std::make_tuple(type, size);
	}

	JSON_LOADER_EXCEPTION("Height/Width value must be either a number or a string.\nLayout Name: {}\nInvalid value: {}", layoutName, data.dump(4));
}
RowColumnPosition LayoutLoader::ParseRowColumnPosition(json& data)
{
	RowColumnPosition position;
	position.Row = 0;
	position.Column = 0;
	position.RowSpan = 1;
	position.ColumnSpan = 1;

	if (data.contains("Row"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Row"].is_number_unsigned(), "'Row' value must be an unsigned int.\nInvalid value: {}", data["Row"].dump(4));
		position.Row = data["Row"].get<unsigned int>();
	}

	if (data.contains("Column"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Column"].is_number_unsigned(), "'Column' value must be an unsigned int.\nInvalid value: {}", data["Column"].dump(4));
		position.Column = data["Column"].get<unsigned int>();
	}

	if (data.contains("RowSpan"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RowSpan"].is_number_unsigned(), "'RowSpan' value must be an unsigned int.\nInvalid value : {}", data["RowSpan"].dump(4));

		position.RowSpan = data["RowSpan"].get<unsigned int>();

		if (position.RowSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'RowSpan' with value of 0. Setting rowSpan = 1", __FILE__, __LINE__);
			position.RowSpan = 1;
		}
	}

	if (data.contains("ColumnSpan"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ColumnSpan"].is_number_unsigned(), "'ColumnSpan' value must be an unsigned int.\nInvalid value : {}", data["ColumnSpan"].dump(4));

		position.ColumnSpan = data["ColumnSpan"].get<unsigned int>();

		if (position.ColumnSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'ColumnSpan' with value of 0. Setting columnSpan = 1", __FILE__, __LINE__);
			position.ColumnSpan = 1;
		}
	}

	return position;
}
}