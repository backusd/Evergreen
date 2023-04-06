#include "pch.h"
#include "ScrollableLayoutLoader.h"





namespace Evergreen
{
Control* ScrollableLayoutLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	m_name = name;
	JSONLoaders::AddControlName(name); // Add control name so we can force names to be unique


	RowColumnPosition rowCol;
	Margin margin;

	// Parse Row/Column
	rowCol = rowColumnPositionOverride.has_value() ? rowColumnPositionOverride.value() : ParseRowColumnPosition(data);

	// Parse Margin
	margin = ParseMargin(data);

	// Parse Brushes (Note: these functions are allowed to return nullptr and if so, the ScrollableLayout will just create a default brush)
	std::unique_ptr<ColorBrush> backgroundBrush = ParseBackgroundBrush(deviceResources, data);
	std::unique_ptr<ColorBrush> borderBrush = ParseBorderBrush(deviceResources, data);

	// Border Width
	float borderWidth = ParseBorderWidth(data);

	// Scroll direction
	bool scrollVertical = ParseScrollVertical(data);
	bool scrollhorizontal = ParseScrollHorizontal(data);

	if (!scrollVertical && !scrollhorizontal)
		EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. 'scrollHorizontal' and 'scrollVertical' are both false. At least one of these should be true.", __FILE__, __LINE__, m_name);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "RowDefinitions", "ColumnDefinitions",
	"Margin", "ScrollVertical", "ScrollHorizontal", "BackgroundBrush", "BorderBrush", "BorderWidth", "VerticalScrollBarCornerRadius", 
	"VerticalScrollBarCornerRadiusX", "VerticalScrollBarCornerRadiusY", "VerticalScrollBarEnabled", "VerticalScrollBarHiddenWhenNotOver", 
	"VerticalScrollBarWidth",	"VerticalScrollBarRegionWidth", "VerticalScrollBarBrush", "VerticalScrollBarHoveredBrush", 
	"VerticalScrollBarDraggingBrush", "HorizontalScrollBarCornerRadius", "HorizontalScrollBarCornerRadiusX", "HorizontalScrollBarCornerRadiusY",
	"HorizontalScrollBarEnabled", "HorizontalScrollBarHiddenWhenNotOver", "HorizontalScrollBarHeight", "HorizontalScrollBarRegionHeight",
	"HorizontalScrollBarBrush", "HorizontalScrollBarHoveredBrush", "HorizontalScrollBarDraggingBrush", "OnUpdate"};
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
		{
			// Ignore the key/value if it is an object. It will be assumed that it is either a Control or Layout object for now, but
			// if there is an issue with the object, it will get picked up by JSONLoaders::LoadLayout()
			if (!data[key].is_object())
				EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
		}
	}

	// Create the new Text control
	ScrollableLayout* scrollableLayout = parent->CreateControl<ScrollableLayout>(
		rowCol, 
		deviceResources, 
		scrollVertical,
		scrollhorizontal,
		std::move(backgroundBrush), 
		std::move(borderBrush), 
		borderWidth, 
		margin);
	EG_CORE_ASSERT(scrollableLayout != nullptr, "Something went wrong, scrollableLayout is nullptr");

	scrollableLayout->Name(name);
	scrollableLayout->ID(ParseID(data));

	ParseRowDefinitions(scrollableLayout, data, scrollVertical);
	ParseColumnDefinitions(scrollableLayout, data, scrollhorizontal);

	// This is a good time to do the layout check because ScrollableLayout will disable the layout check when rows/columns are added
	scrollableLayout->GetLayout()->LayoutCheck();

	ParseVerticalScrollBarCornerRadius(scrollableLayout, data);
	ParseVerticalScrollBarEnabled(scrollableLayout, data);
	ParseVerticalScrollBarHiddenWhenNotOver(scrollableLayout, data);
	ParseVerticalScrollBarWidth(scrollableLayout, data);
	ParseVerticalScrollBarRegionWidth(scrollableLayout, data);
	ParseVerticalScrollBarBrush(deviceResources, scrollableLayout, data);
	ParseVerticalScrollBarHoveredBrush(deviceResources, scrollableLayout, data);
	ParseVerticalScrollBarDraggingBrush(deviceResources, scrollableLayout, data);

	ParseHorizontalScrollBarCornerRadius(scrollableLayout, data);
	ParseHorizontalScrollBarEnabled(scrollableLayout, data);
	ParseHorizontalScrollBarHiddenWhenNotOver(scrollableLayout, data);
	ParseHorizontalScrollBarHeight(scrollableLayout, data);
	ParseHorizontalScrollBarRegionHeight(scrollableLayout, data);
	ParseHorizontalScrollBarBrush(deviceResources, scrollableLayout, data);
	ParseHorizontalScrollBarHoveredBrush(deviceResources, scrollableLayout, data);
	ParseHorizontalScrollBarDraggingBrush(deviceResources, scrollableLayout, data);

	LoadControls(deviceResources, scrollableLayout, data);

	ParseOnUpdateCallback(scrollableLayout, data);

	return scrollableLayout;
}
std::unique_ptr<ColorBrush> ScrollableLayoutLoader::ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BackgroundBrush"), "ScrollableLayout control with name '{}': No 'BackgroundBrush' key. Incomplete ScrollableLayout object: {}", m_name, data.dump(4));

	return std::move(JSONLoaders::LoadBrush(deviceResources, data["BackgroundBrush"]));
}
std::unique_ptr<ColorBrush> ScrollableLayoutLoader::ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// Not including a 'BorderBrush' key is only invalid if a 'BorderWidth' greater than 0 has been specified
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].is_number(), "ScrollableLayout control with name '{}': Parsing BorderBrush, but noticed that 'BorderWidth' value is not a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float borderWidth = data["BorderWidth"].get<float>();
		if (borderWidth > 0.0f)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BorderBrush"), "ScrollableLayout control with name '{}': 'BorderWidth' is non-zero, but no 'BorderBrush' has been specified. Incomplete ScrollableLayout object: {}", m_name, data.dump(4));
		}
	}

	if (data.contains("BorderBrush"))
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"]));

	// Note: we are allowed to return nullptr here because the Button class will just create a default border brush
	return nullptr;
}
float ScrollableLayoutLoader::ParseBorderWidth(json& data)
{
	if (data.contains("BorderWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].is_number(), "ScrollableLayout control with name '{}': 'BorderWidth' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float borderWidth = data["BorderWidth"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(borderWidth >= 0.0f, "ScrollableLayout control with name '{}': 'BorderWidth' is not allowed to be less than 0. Incomplete ScrollableLayout object: {}", m_name, data.dump(4));

		return borderWidth;
	}

	return 0.0f;
}

bool ScrollableLayoutLoader::ParseScrollVertical(json& data)
{
	if (data.contains("ScrollVertical"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ScrollVertical"].is_boolean(), "ScrollableLayout control with name '{}': 'ScrollVertical' value must be a boolean. Invalid ScrollableLayout object: {}", m_name, data.dump(4));
		return data["ScrollVertical"].get<bool>();
	}

	return false;
}
bool ScrollableLayoutLoader::ParseScrollHorizontal(json& data)
{
	if (data.contains("ScrollHorizontal"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ScrollHorizontal"].is_boolean(), "ScrollableLayout control with name '{}': 'ScrollHorizontal' value must be a boolean. Invalid ScrollableLayout object: {}", m_name, data.dump(4));
		return data["ScrollHorizontal"].get<bool>();
	}

	return false;
}

void ScrollableLayoutLoader::ParseRowDefinitions(ScrollableLayout* scrollableLayout, json& data, bool canScrollVertical)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	// If 'RowDefinitions' is not present, make sure it is not vertically scrollable and add a single star row
	if (!data.contains("RowDefinitions"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!canScrollVertical, "ScrollableLayout control with name '{}': 'scrollvertical' is true, but no 'RowDefinitions' is found: {}", m_name, data.dump(4));

		// Add a single row that spans the layout
		scrollableLayout->AddRow({ RowColumnType::STAR, 1.0f });
		return;
	}

	JSON_LOADER_EXCEPTION_IF_FALSE(data["RowDefinitions"].is_array(), "ScrollableLayout control with name '{}': RowDefinitions value must be an array type. Invalid value: {}", m_name, data["RowDefinitions"].dump(4));

	// Iterate the list of row definitions
	for (auto& rowDefinition : data["RowDefinitions"])
	{
		// Each row must contain a 'Height' key
		JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition.contains("Height"), "ScrollableLayout control with name '{}': RowDefinition does not contain 'Height' key: {}", m_name, rowDefinition.dump(4));

		// Get the Row type and size
		auto [rowColType, rowColSize] = JSONLoaders::ParseRowColumnTypeAndSize(rowDefinition["Height"], m_name);

		// If we can scroll vertically, the row type MUST be fixed
		if (canScrollVertical)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(rowColType == RowColumnType::FIXED, "ScrollableLayout control with name '{}': 'scrollVertical' is true, therefore all 'RowDefinitions' must have FIXED type. Invalid RowDefinitions: {}", m_name, data["RowDefinitions"].dump(4));
		}

		// Create the row and get a pointer to it so we can edit it further
		Row* row = scrollableLayout->AddRow({ rowColType, rowColSize });

		// Before looping over all the row definition key/value pairs, first check if we need to import other json data
		JSONLoaders::ImportJSON(rowDefinition);

		// Iterate over the other keys in the row definition
		for (auto& [key, value] : rowDefinition.items())
		{
			if (key.compare("Height") == 0 || key.compare("comment") == 0 || key.compare("import") == 0)
				continue;
			else if (key.compare("TopAdjustable") == 0)
			{
				JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition[key].is_boolean(), "ScrollableLayout control with name '{}': RowDefinition TopAdjustable field must have boolean type.\nInvalid value: {}", m_name, rowDefinition[key].dump(4));
				row->TopIsAdjustable(rowDefinition[key].get<bool>());
			}
			else if (key.compare("BottomAdjustable") == 0)
			{
				JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition[key].is_boolean(), "ScrollableLayout control with name '{}': RowDefinition BottomAdjustable field must have boolean type.\nInvalid value: {}", m_name, rowDefinition[key].dump(4));
				row->BottomIsAdjustable(rowDefinition[key].get<bool>());
			}
			else if (key.compare("MaxHeight") == 0)
			{
				auto [type, size] = JSONLoaders::ParseRowColumnTypeAndSize(rowDefinition[key], m_name);
				row->MaxHeight({ type, size });
			}
			else if (key.compare("MinHeight") == 0)
			{
				auto [type, size] = JSONLoaders::ParseRowColumnTypeAndSize(rowDefinition[key], m_name);
				row->MinHeight({ type, size });
			}
			else
			{
				// Just a warning because the key will be ignored
				EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}': Unrecognized key ({}) in RowDefinition.", __FILE__, __LINE__, m_name, key);
			}
		}
	}

}
void ScrollableLayoutLoader::ParseColumnDefinitions(ScrollableLayout* scrollableLayout, json& data, bool canScrollHorizontal)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	// If 'ColumnDefinitions' is not present, make sure it is not vertically scrollable and add a single star row
	if (!data.contains("ColumnDefinitions"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!canScrollHorizontal, "ScrollableLayout control with name '{}': 'scrollHorizontal' is true, but no 'ColumnDefinitions' is found: {}", m_name, data.dump(4));

		// Add a single column that spans the layout
		scrollableLayout->AddColumn({ RowColumnType::STAR, 1.0f });
		return;
	}

	JSON_LOADER_EXCEPTION_IF_FALSE(data["ColumnDefinitions"].is_array(), "ScrollableLayout control with name '{}': ColumnDefinitions value must be an array type. Invalid value: {}", m_name, data["ColumnDefinitions"].dump(4));

	// Iterate the list of column definitions
	for (auto& columnDefinition : data["ColumnDefinitions"])
	{
		// Each column must contain a 'Width' key
		JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition.contains("Width"), "ScrollableLayout control with name '{}': ColumnDefinition does not contain 'Width' key: {}", m_name, columnDefinition.dump(4));

		// Get the Column type and size
		auto [rowColType, rowColSize] = JSONLoaders::ParseRowColumnTypeAndSize(columnDefinition["Width"], m_name);

		// If we can scroll horizontally, the column type MUST be fixed
		if (canScrollHorizontal)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(rowColType == RowColumnType::FIXED, "ScrollableLayout control with name '{}': 'scrollHorizontal' is true, therefore all 'ColumnDefinitions' must have FIXED type. Invalid ColumnDefinitions: {}", m_name, data["ColumnDefinitions"].dump(4));
		}

		// Create the row and get a pointer to it so we can edit it further
		Column* column = scrollableLayout->AddColumn({ rowColType, rowColSize });

		// Before looping over all the row definition key/value pairs, first check if we need to import other json data
		JSONLoaders::ImportJSON(columnDefinition);

		// Iterate over the other keys in the column definition
		for (auto& [key, value] : columnDefinition.items())
		{
			if (key.compare("Width") == 0 || key.compare("comment") == 0 || key.compare("import") == 0)
				continue;
			else if (key.compare("LeftAdjustable") == 0)
			{
				JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition[key].is_boolean(), "ScrollableLayout control with name '{}': ColumnDefinition LeftAdjustable field must have boolean type.\nInvalid value: {}", m_name, columnDefinition[key].dump(4));
				column->LeftIsAdjustable(columnDefinition[key].get<bool>());
			}
			else if (key.compare("RightAdjustable") == 0)
			{
				JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition[key].is_boolean(), "ScrollableLayout control with name '{}': ColumnDefinition RightAdjustable field must have boolean type.\nInvalid value: {}", m_name, columnDefinition[key].dump(4));
				column->RightIsAdjustable(columnDefinition[key].get<bool>());
			}
			else if (key.compare("MaxWidth") == 0)
			{
				auto [type, size] = JSONLoaders::ParseRowColumnTypeAndSize(columnDefinition[key], m_name);
				column->MaxWidth({ type, size });
			}
			else if (key.compare("MinWidth") == 0)
			{
				auto [type, size] = JSONLoaders::ParseRowColumnTypeAndSize(columnDefinition[key], m_name);
				column->MinWidth({ type, size });
			}
			else
			{
				// Just a warning because the key will be ignored
				EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}': Unrecognized key ({}) in ColumnDefinition.", __FILE__, __LINE__, m_name, key);
			}
		}
	}
}


void ScrollableLayoutLoader::ParseVerticalScrollBarCornerRadius(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	// We support either specifying 'VerticalScrollBarCornerRadius' which will set both the X and Y radii, or
	// specifying 'VerticalScrollBarCornerRadiusX' and 'VerticalScrollBarCornerRadiusY'. A warning will appear
	// if an invalid combination is found.
	if (data.contains("VerticalScrollBarCornerRadius"))
	{
		if (data.contains("VerticalScrollBarCornerRadiusX"))
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Key 'VerticalScrollBarCornerRadiusX' should only be used if NOT using 'VerticalScrollBarCornerRadius'. This key is being IGNORED", __FILE__, __LINE__, m_name);
		}
		if (data.contains("VerticalScrollBarCornerRadiusY"))
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Key 'VerticalScrollBarCornerRadiusY' should only be used if NOT using 'VerticalScrollBarCornerRadius'. This key is being IGNORED", __FILE__, __LINE__, m_name);
		}

		JSON_LOADER_EXCEPTION_IF_FALSE(data["VerticalScrollBarCornerRadius"].is_number(), "ScrollableLayout control with name '{}': 'VerticalScrollBarCornerRadius' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float radius = data["VerticalScrollBarCornerRadius"].get<float>();

		if (radius <= 0.0f)
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. 'VerticalScrollBarCornerRadius' value should be greater than 0. Ignoring value: {}", __FILE__, __LINE__, m_name, radius);
			radius = 0.0f;
		}

		scrollableLayout->VerticalScrollBarCornerRadius(radius);
	}
	else if (data.contains("VerticalScrollBarCornerRadiusX") && data.contains("VerticalScrollBarCornerRadiusY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["VerticalScrollBarCornerRadiusX"].is_number(), "ScrollableLayout control with name '{}': 'VerticalScrollBarCornerRadiusX' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["VerticalScrollBarCornerRadiusY"].is_number(), "ScrollableLayout control with name '{}': 'VerticalScrollBarCornerRadiusY' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float radiusX = data["VerticalScrollBarCornerRadiusX"].get<float>();
		if (radiusX <= 0.0f)
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. 'VerticalScrollBarCornerRadiusX' value should be greater than 0. Ignoring value: {}", __FILE__, __LINE__, m_name, radiusX);
			radiusX = 0.0f;
		}

		float radiusY = data["VerticalScrollBarCornerRadiusY"].get<float>();
		if (radiusY <= 0.0f)
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. 'VerticalScrollBarCornerRadiusY' value should be greater than 0. Ignoring value: {}", __FILE__, __LINE__, m_name, radiusY);
			radiusY = 0.0f;
		}

		scrollableLayout->VerticalScrollBarCornerRadius(radiusX, radiusY);
	}
	else if (data.contains("VerticalScrollBarCornerRadiusX"))
	{
		EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Key 'VerticalScrollBarCornerRadiusX' was found but key 'VerticalScrollBarCornerRadiusY' was not. Both must be specified to produce rounded corners (or just use key 'VerticalScrollBarCornerRadius' to set both X and Y radii).", __FILE__, __LINE__, m_name);
	}
	else if (data.contains("VerticalScrollBarCornerRadiusY"))
	{
		EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Key 'VerticalScrollBarCornerRadiusY' was found but key 'VerticalScrollBarCornerRadiusX' was not. Both must be specified to produce rounded corners (or just use key 'VerticalScrollBarCornerRadius' to set both X and Y radii).", __FILE__, __LINE__, m_name);
	}
}
void ScrollableLayoutLoader::ParseVerticalScrollBarEnabled(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("VerticalScrollBarEnabled"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["VerticalScrollBarEnabled"].is_boolean(), "ScrollableLayout control with name '{}': 'VerticalScrollBarEnabled' value must be a boolean. Invalid ScrollableLayout object: {}", m_name, data.dump(4));
		scrollableLayout->VerticalScrollBarEnabled(data["VerticalScrollBarEnabled"].get<bool>());
	}
}
void ScrollableLayoutLoader::ParseVerticalScrollBarHiddenWhenNotOver(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("VerticalScrollBarHiddenWhenNotOver"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["VerticalScrollBarHiddenWhenNotOver"].is_boolean(), "ScrollableLayout control with name '{}': 'VerticalScrollBarHiddenWhenNotOver' value must be a boolean. Invalid ScrollableLayout object: {}", m_name, data.dump(4));
		scrollableLayout->VerticalScrollBarHiddenWhenNotOver(data["VerticalScrollBarHiddenWhenNotOver"].get<bool>());
	}
}
void ScrollableLayoutLoader::ParseVerticalScrollBarWidth(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("VerticalScrollBarWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["VerticalScrollBarWidth"].is_number(), "ScrollableLayout control with name '{}': 'VerticalScrollBarWidth' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float width = data["VerticalScrollBarWidth"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "ScrollableLayout control with name '{}': 'VerticalScrollBarWidth' must be greater than 0. Incomplete ScrollableLayout object: {}", m_name, data.dump(4));

		scrollableLayout->VerticalScrollBarWidth(width);
	}
}
void ScrollableLayoutLoader::ParseVerticalScrollBarRegionWidth(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("VerticalScrollBarRegionWidth"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["VerticalScrollBarRegionWidth"].is_number(), "ScrollableLayout control with name '{}': 'VerticalScrollBarRegionWidth' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float width = data["VerticalScrollBarRegionWidth"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "ScrollableLayout control with name '{}': 'VerticalScrollBarRegionWidth' must be greater than 0. Incomplete ScrollableLayout object: {}", m_name, data.dump(4));

		scrollableLayout->VerticalScrollBarRegionWidth(width);
	}
}
void ScrollableLayoutLoader::ParseVerticalScrollBarBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("VerticalScrollBarBrush"))
	{
		scrollableLayout->VerticalScrollBarBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["VerticalScrollBarBrush"])));
	}
}
void ScrollableLayoutLoader::ParseVerticalScrollBarHoveredBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("VerticalScrollBarHoveredBrush"))
	{
		scrollableLayout->VerticalScrollBarHoveredBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["VerticalScrollBarHoveredBrush"])));
	}
}
void ScrollableLayoutLoader::ParseVerticalScrollBarDraggingBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("VerticalScrollBarDraggingBrush"))
	{
		scrollableLayout->VerticalScrollBarDraggingBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["VerticalScrollBarDraggingBrush"])));
	}
}


void ScrollableLayoutLoader::ParseHorizontalScrollBarCornerRadius(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	// We support either specifying 'HorizontalScrollBarCornerRadius' which will set both the X and Y radii, or
	// specifying 'HorizontalScrollBarCornerRadiusX' and 'HorizontalScrollBarCornerRadiusY'. A warning will appear
	// if an invalid combination is found.
	if (data.contains("HorizontalScrollBarCornerRadius"))
	{
		if (data.contains("HorizontalScrollBarCornerRadiusX"))
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Key 'HorizontalScrollBarCornerRadiusX' should only be used if NOT using 'HorizontalScrollBarCornerRadius'. This key is being IGNORED", __FILE__, __LINE__, m_name);
		}
		if (data.contains("HorizontalScrollBarCornerRadiusY"))
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Key 'HorizontalScrollBarCornerRadiusY' should only be used if NOT using 'HorizontalScrollBarCornerRadius'. This key is being IGNORED", __FILE__, __LINE__, m_name);
		}

		JSON_LOADER_EXCEPTION_IF_FALSE(data["HorizontalScrollBarCornerRadius"].is_number(), "ScrollableLayout control with name '{}': 'HorizontalScrollBarCornerRadius' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float radius = data["HorizontalScrollBarCornerRadius"].get<float>();

		if (radius <= 0.0f)
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. 'HorizontalScrollBarCornerRadius' value should be greater than 0. Ignoring value: {}", __FILE__, __LINE__, m_name, radius);
			radius = 0.0f;
		}

		scrollableLayout->HorizontalScrollBarCornerRadius(radius);
	}
	else if (data.contains("HorizontalScrollBarCornerRadiusX") && data.contains("HorizontalScrollBarCornerRadiusY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["HorizontalScrollBarCornerRadiusX"].is_number(), "ScrollableLayout control with name '{}': 'HorizontalScrollBarCornerRadiusX' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["HorizontalScrollBarCornerRadiusY"].is_number(), "ScrollableLayout control with name '{}': 'HorizontalScrollBarCornerRadiusY' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float radiusX = data["HorizontalScrollBarCornerRadiusX"].get<float>();
		if (radiusX <= 0.0f)
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. 'HorizontalScrollBarCornerRadiusX' value should be greater than 0. Ignoring value: {}", __FILE__, __LINE__, m_name, radiusX);
			radiusX = 0.0f;
		}

		float radiusY = data["HorizontalScrollBarCornerRadiusY"].get<float>();
		if (radiusY <= 0.0f)
		{
			EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. 'HorizontalScrollBarCornerRadiusY' value should be greater than 0. Ignoring value: {}", __FILE__, __LINE__, m_name, radiusY);
			radiusY = 0.0f;
		}

		scrollableLayout->HorizontalScrollBarCornerRadius(radiusX, radiusY);
	}
	else if (data.contains("HorizontalScrollBarCornerRadiusX"))
	{
		EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Key 'HorizontalScrollBarCornerRadiusX' was found but key 'HorizontalScrollBarCornerRadiusY' was not. Both must be specified to produce rounded corners (or just use key 'HorizontalScrollBarCornerRadius' to set both X and Y radii).", __FILE__, __LINE__, m_name);
	}
	else if (data.contains("HorizontalScrollBarCornerRadiusY"))
	{
		EG_CORE_WARN("{}:{} - ScrollableLayout control with name '{}'. Key 'HorizontalScrollBarCornerRadiusY' was found but key 'HorizontalScrollBarCornerRadiusX' was not. Both must be specified to produce rounded corners (or just use key 'HorizontalScrollBarCornerRadius' to set both X and Y radii).", __FILE__, __LINE__, m_name);
	}
}
void ScrollableLayoutLoader::ParseHorizontalScrollBarEnabled(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("HorizontalScrollBarEnabled"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["HorizontalScrollBarEnabled"].is_boolean(), "ScrollableLayout control with name '{}': 'HorizontalScrollBarEnabled' value must be a boolean. Invalid ScrollableLayout object: {}", m_name, data.dump(4));
		scrollableLayout->HorizontalScrollBarEnabled(data["HorizontalScrollBarEnabled"].get<bool>());
	}
}
void ScrollableLayoutLoader::ParseHorizontalScrollBarHiddenWhenNotOver(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("HorizontalScrollBarHiddenWhenNotOver"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["HorizontalScrollBarHiddenWhenNotOver"].is_boolean(), "ScrollableLayout control with name '{}': 'HorizontalScrollBarHiddenWhenNotOver' value must be a boolean. Invalid ScrollableLayout object: {}", m_name, data.dump(4));
		scrollableLayout->HorizontalScrollBarHiddenWhenNotOver(data["HorizontalScrollBarHiddenWhenNotOver"].get<bool>());
	}
}
void ScrollableLayoutLoader::ParseHorizontalScrollBarHeight(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("HorizontalScrollBarHeight"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["HorizontalScrollBarHeight"].is_number(), "ScrollableLayout control with name '{}': 'HorizontalScrollBarHeight' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float height = data["HorizontalScrollBarHeight"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(height >= 0.0f, "ScrollableLayout control with name '{}': 'HorizontalScrollBarHeight' must be greater than 0. Incomplete ScrollableLayout object: {}", m_name, data.dump(4));

		scrollableLayout->HorizontalScrollBarHeight(height);
	}
}
void ScrollableLayoutLoader::ParseHorizontalScrollBarRegionHeight(ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");
	
	if (data.contains("HorizontalScrollBarRegionHeight"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["HorizontalScrollBarRegionHeight"].is_number(), "ScrollableLayout control with name '{}': 'HorizontalScrollBarRegionHeight' value must be a number. Invalid ScrollableLayout object: {}", m_name, data.dump(4));

		float height = data["HorizontalScrollBarRegionHeight"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(height >= 0.0f, "ScrollableLayout control with name '{}': 'HorizontalScrollBarRegionHeight' must be greater than 0. Incomplete ScrollableLayout object: {}", m_name, data.dump(4));

		scrollableLayout->HorizontalScrollBarRegionHeight(height);
	}
}
void ScrollableLayoutLoader::ParseHorizontalScrollBarBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("HorizontalScrollBarBrush"))
	{
		scrollableLayout->HorizontalScrollBarBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["HorizontalScrollBarBrush"])));
	}
}
void ScrollableLayoutLoader::ParseHorizontalScrollBarHoveredBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("HorizontalScrollBarHoveredBrush"))
	{
		scrollableLayout->HorizontalScrollBarHoveredBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["HorizontalScrollBarHoveredBrush"])));
	}
}
void ScrollableLayoutLoader::ParseHorizontalScrollBarDraggingBrush(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	if (data.contains("HorizontalScrollBarDraggingBrush"))
	{
		scrollableLayout->HorizontalScrollBarDraggingBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["HorizontalScrollBarDraggingBrush"])));
	}
}

void ScrollableLayoutLoader::LoadControls(std::shared_ptr<DeviceResources> deviceResources, ScrollableLayout* scrollableLayout, json& data)
{
	EG_CORE_ASSERT(scrollableLayout != nullptr, "ScrollableLayout should not be nullptr");

	constexpr std::array recognizedKeys{ "Type", "Row", "Column", "RowSpan", "ColumnSpan", "RowDefinitions", "ColumnDefinitions",
	"Margin", "ScrollVertical", "ScrollHorizontal", "BackgroundBrush", "BorderBrush", "BorderWidth", "VerticalScrollBarCornerRadius",
	"VerticalScrollBarCornerRadiusX", "VerticalScrollBarCornerRadiusY", "VerticalScrollBarEnabled", "VerticalScrollBarHiddenWhenNotOver",
	"VerticalScrollBarWidth",	"VerticalScrollBarRegionWidth", "VerticalScrollBarBrush", "VerticalScrollBarHoveredBrush",
	"VerticalScrollBarDraggingBrush", "HorizontalScrollBarCornerRadius", "HorizontalScrollBarCornerRadiusX", "HorizontalScrollBarCornerRadiusY",
	"HorizontalScrollBarEnabled", "HorizontalScrollBarHiddenWhenNotOver", "HorizontalScrollBarHeight", "HorizontalScrollBarRegionHeight",
	"HorizontalScrollBarBrush", "HorizontalScrollBarHoveredBrush", "HorizontalScrollBarDraggingBrush" };	

	// Now iterate over the controls and sublayouts within the layout
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) != recognizedKeys.end())
			continue;

		JSON_LOADER_EXCEPTION_IF_FALSE(data[key].contains("Type"), "ScrollableLayout control with name '{}': Child control has no 'Type' definition: {}", m_name, data[key].dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data[key]["Type"].is_string(), "ScrollableLayout control with name '{}': Child control 'Type' value must be a string.\nInvalid value : {}", m_name, data[key]["Type"].dump(4));

		std::string type = data[key]["Type"].get<std::string>();

		// Load either a sublayout or control
		if (JSONLoaders::IsControlKey(type))
		{
			JSONLoaders::ImportJSON(data[key]);
			Control* control = JSONLoaders::LoadControl(deviceResources, type, scrollableLayout->GetLayout(), data[key], key);
			JSON_LOADER_EXCEPTION_IF_FALSE(control != nullptr, "ScrollableLayout control with name '{}': Failed to load child control: {}", m_name, key);
		}
		else
		{
			EG_CORE_WARN("ScrollableLayout control with name '{}': Attempting to load control: {} (... not yet supported ...)", m_name, type);
		}
	}

}
}