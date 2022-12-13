#include "pch.h"
#include "UI.h"
#include "ControlLoaders/TextLoader.h"
#include "BrushLoaders/SolidColorBrushLoader.h"

#include <fstream>

#include "Colors/SolidColorBrush.h"
#include "Colors/GradientBrush.h"
#include "Colors/RadialBrush.h"
#include "Colors/BitmapBrush.h"


#define UI_ERROR(fmt, ...) m_errorMessages.push_back(std::format(fmt, __VA_ARGS__)); ::Evergreen::Log::GetCoreLogger()->error(std::format(CAT2("{}:{} - ", fmt), __FILE__, __LINE__, __VA_ARGS__))

namespace Evergreen
{
std::unordered_map<std::string, std::function<bool(std::shared_ptr<DeviceResources>, Layout*, json&, const std::string&, GlobalJsonData*)>>			UI::m_loadControlFunctions;
std::unordered_map<std::string, std::function<std::optional<std::shared_ptr<Style>>(std::shared_ptr<DeviceResources>, json&, const std::string&)>>	UI::m_loadStyleFunctions;
//std::unordered_map<std::string, std::function<std::optional<std::unique_ptr<ColorBrush>>(std::shared_ptr<DeviceResources>, json&)>>					UI::m_loadColorBrushFunctions;


UI::UI(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<Window> window) noexcept :
	m_deviceResources(deviceResources),
	m_window(window),
	m_jsonRoot({}),
	m_rootLayout(nullptr),
	m_globalJsonData(std::make_shared<GlobalJsonData>())
{
	// Add built-in control loaders
	UI::SetControlLoaderFunction("Text", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, GlobalJsonData* globalData) -> bool { return TextLoader::Load(deviceResources, parentLayout, data, controlName, globalData); });

	// Add built-in style loaders
	UI::SetStyleLoaderFunction("TextStyle", [](std::shared_ptr<DeviceResources> deviceResources, json& data, const std::string& styleName) -> std::optional<std::shared_ptr<Style>> { return TextStyleLoader::Load(deviceResources, data, styleName); });

	// Add built-in brush loaders
	//UI::SetBrushLoaderFunction("SolidColorBrush", [](std::shared_ptr<DeviceResources> deviceResources, json& data) -> std::optional<std::unique_ptr<ColorBrush>> { return SolidColorBrushLoader::Load(deviceResources, data); });


	LoadDefaultUI();
}

void UI::LoadDefaultUI() noexcept
{
	// TODO: Load a default UI that displays a "No defined layout"
	// Could make this quite elaborate, similar to how ImGui has the example of all controls
	// This could also contain links our website/documentation



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

	if (std::optional<Column*> column = m_rootLayout->AddColumn({ RowColumnType::FIXED, 100.0f }))
	{
		column.value()->RightIsAdjustable(true);
	}
	if (std::optional<Column*> column = m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f }))
	{
		column.value()->LeftIsAdjustable(true);
	}

	std::wstring textString = L"Some very long test text";
	std::shared_ptr<TextStyle> defaultStyle = std::make_shared<TextStyle>(m_deviceResources);
	std::shared_ptr<TextStyle> style = std::make_shared<TextStyle>(
		m_deviceResources,
		"Custom Style",
		std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black, 1.0f))),
		Evergreen::FontFamily::Arial,
		22.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD
	);
	if (std::optional<Text*> text = m_rootLayout->AddControl<Text>(m_deviceResources, textString, defaultStyle))
	{
		EG_CORE_TRACE("{}", "Successfully created default text");
	}
	else
	{
		EG_CORE_ERROR("{}", "Failed to create default text");
	}

	RowColumnPosition position;
	position.Row = 1;
	position.Column = 0;
	position.RowSpan = 1;
	position.ColumnSpan = 1;

	std::wstring s = L"Custom Text";

	if (std::optional<Text*> text = m_rootLayout->AddControl<Text>(position, m_deviceResources, s, style))
	{
		EG_CORE_TRACE("{}", "Successfully created custom text");
	}
	else
	{
		EG_CORE_ERROR("{}", "Failed to create custom text");
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
			UI_ERROR("File {} has no 'root' key", filePath.string());
			LoadErrorUI();
			return;
		}

		// Set m_jsonRoot so we can access non-root keys for import within the Load methods
		m_jsonRoot = data.value();

		// Before constructing the layout, load global data that may be needed by Load* functions
		if (!ParseGlobalStyles(m_jsonRoot))
		{
			UI_ERROR("{}", "Call to ParseGlobalStyles failed");
			LoadErrorUI();
			return;
		}

		// Get the root layout data and create the root layout
		json rootLayoutData = m_jsonRoot["root"];

		m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()), "Root Layout");
		if (!LoadLayoutDetails(m_rootLayout.get(), rootLayoutData))
		{
			m_jsonRoot = {};
			m_globalJsonData = nullptr;
			LoadErrorUI();
			return;
		}

		m_globalJsonData = nullptr;

		// LayoutCheck is entirely optional - In a Release build, this does nothing
		m_rootLayout->LayoutCheck();
	}
	else
	{
		m_jsonRoot = {};
		m_globalJsonData = nullptr;
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
			UI_ERROR("File: {}\njson::parse_error: {}", filePath.string(), e.what());
			return std::nullopt;
		}
	}

	UI_ERROR("Failed to open file: {}", filePath.string());
	return std::nullopt;
}

void UI::LoadErrorUI() noexcept
{
	m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()));
	m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f });
	m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f });
	
	std::shared_ptr<TextStyle> style = std::make_shared<TextStyle>(
		m_deviceResources,
		"Error Style",
		std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black, 1.0f))),
		Evergreen::FontFamily::Arial,
		22.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD
	);

	std::wstring text = L"";
	std::wstring message;

	for (const std::string& s : m_errorMessages)
		text = std::format(L"{}\n{}\n", text, std::wstring(s.begin(), s.end()));

	m_rootLayout->AddControl<Text>(m_deviceResources, text, style);
}

void UI::Render(DeviceResources* deviceResources) const noexcept
{
	deviceResources->BeginDraw();

	m_rootLayout->Render(deviceResources);





	//SolidColorBrush brush(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue, 1.0f));


	std::vector<D2D1_GRADIENT_STOP> stops;
	stops.emplace_back(0.0f, D2D1::ColorF(D2D1::ColorF::DarkBlue, 1.0f));
	stops.emplace_back(0.5f, D2D1::ColorF(D2D1::ColorF::Blue, 1.0f));
	stops.emplace_back(1.0f, D2D1::ColorF(D2D1::ColorF::LightBlue, 1.0f));

	GradientBrush brush(
		m_deviceResources,
		stops,
		GRADIENT_AXIS::NWSE
	);

	D2D1_RECT_F rect{ 100.0f, 100.0f, 500.0f, 500.0f };
	brush.SetDrawRegion(rect);
	m_deviceResources->D2DDeviceContext()->FillRectangle(&rect, brush.Get());

	// -------------

	std::vector<D2D1_GRADIENT_STOP> stops2;
	stops2.emplace_back(0.0f, D2D1::ColorF(D2D1::ColorF::Red, 1.0f));
	stops2.emplace_back(0.5f, D2D1::ColorF(D2D1::ColorF::Blue, 1.0f));
	stops2.emplace_back(1.0f, D2D1::ColorF(D2D1::ColorF::Green, 1.0f));

	RadialBrush radialBrush(
		m_deviceResources, 
		stops2
	);

	D2D1_ELLIPSE ellipse = D2D1::Ellipse({ 600.0f, 600.0f }, 75.0f, 75.0f);
	D2D1_RECT_F rect22 = D2D1::RectF(525.0f, 525.0f, 675.0f, 675.0f);
	radialBrush.SetDrawRegion(rect22);
	m_deviceResources->D2DDeviceContext()->FillEllipse(ellipse, radialBrush.Get());
	// -----------------

	D2D1_RECT_F rect2;
	rect2.bottom = 450.0f;
	rect2.left = 750.0f;
	rect2.top = 200.0f;
	rect2.right = 1050.0f;

	BitmapBrush bmbrush(m_deviceResources, L"evergreen-image-1.jpg");
	bmbrush.TransformToRect(rect2, TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_UNDERFILL_RECT);
	bmbrush.ExtendMode(D2D1_EXTEND_MODE_WRAP);

	m_deviceResources->D2DDeviceContext()->FillRectangle(
		&rect2,
		bmbrush.Get()
	);




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
			UI_ERROR("Control or sub-layout has no 'Type' definition: {}", data[key]);
			return false;
		}

		if (!data[key]["Type"].is_string())
		{
			UI_ERROR("Control or sub-layout 'Type' definition must be a string.\nInvalid value : {}", data[key]["Type"]);
			return false;
		}

		std::string type = data[key]["Type"].get<std::string>();

		// Load either a sublayout or control
		if (type.compare("Layout") == 0)
		{
			if (!LoadSubLayout(layout, data[key], key))
				return false;
		}
		else if (m_loadControlFunctions.find(type) != m_loadControlFunctions.end())
		{
			if (data[key].contains("import"))
			{
				if (!ImportJSON(data[key]))
					return false;
			}

			m_loadControlFunctions[type](m_deviceResources, layout, data[key], key, m_globalJsonData.get());
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
					UI_ERROR("RowDefinition for layout '{}' does not contain 'Height' key: {}", layout->Name(), rowDefinition);
					return false;
				}

				// Get the Row type and size
				std::optional<std::tuple<RowColumnType, float>> typeAndSizeOpt = ParseRowColumnTypeAndSize(rowDefinition["Height"], layout);
				if (!typeAndSizeOpt.has_value())
					return false;

				RowColumnType rowColType = std::get<0>(typeAndSizeOpt.value());
				float rowColSize = std::get<1>(typeAndSizeOpt.value());

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
								UI_ERROR("RowDefinition TopAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), rowDefinition[key]);
								return false;
							}
						}
						else if (key.compare("BottomAdjustable") == 0)
						{
							if (rowDefinition[key].is_boolean())
								row.value()->BottomIsAdjustable(rowDefinition[key].get<bool>());
							else
							{
								UI_ERROR("RowDefinition BottomAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), rowDefinition[key]);
								return false;
							}
						}
						else if (key.compare("MaxHeight") == 0)
						{
							if (std::optional<std::tuple<RowColumnType, float>> typeSizeTupleOpt = ParseRowColumnTypeAndSize(rowDefinition[key], layout))
								row.value()->MaxHeight({ std::get<0>(typeSizeTupleOpt.value()), std::get<1>(typeSizeTupleOpt.value()) });
							else
								return false;
						}
						else if (key.compare("MinHeight") == 0)
						{
							if (std::optional<std::tuple<RowColumnType, float>> typeSizeTupleOpt = ParseRowColumnTypeAndSize(rowDefinition[key], layout))
								row.value()->MinHeight({ std::get<0>(typeSizeTupleOpt.value()), std::get<1>(typeSizeTupleOpt.value()) });
							else
								return false;
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
					UI_ERROR("Failed to add a row for layout with name: {}", layout->Name());
					UI_ERROR("Intended type: {} - Intended height: {}", rowColType, rowColSize);
					return false;
				}
			}
		}
		else
		{
			UI_ERROR("RowDefinitions value must be an array type. Invalid value: {}", data["RowDefinitions"]);
			return false;
		}
	}
	else
	{
		// Add a single row that spans the layout
		if (!layout->AddRow({ RowColumnType::STAR, 1.0f }))
		{
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
					UI_ERROR("ColumnDefinition for layout '{}' does not contain 'Width' key: {}", layout->Name(), columnDefinition);
					return false;
				}

				// Get the Row type and size
				std::optional<std::tuple<RowColumnType, float>> typeAndSizeOpt = ParseRowColumnTypeAndSize(columnDefinition["Width"], layout);
				if (!typeAndSizeOpt.has_value())
					return false;

				RowColumnType rowColType = std::get<0>(typeAndSizeOpt.value());
				float rowColSize = std::get<1>(typeAndSizeOpt.value());

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
								UI_ERROR("ColumnDefinition LeftAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), columnDefinition[key]);
								return false;
							}
						}
						else if (key.compare("RightAdjustable") == 0)
						{
							if (columnDefinition[key].is_boolean())
								column.value()->RightIsAdjustable(columnDefinition[key].get<bool>());
							else
							{
								UI_ERROR("ColumnDefinition RightAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), columnDefinition[key]);
								return false;
							}
						}
						else if (key.compare("MaxWidth") == 0)
						{
							if (std::optional<std::tuple<RowColumnType, float>> typeSizeTupleOpt = ParseRowColumnTypeAndSize(columnDefinition[key], layout))
								column.value()->MaxWidth({ std::get<0>(typeSizeTupleOpt.value()), std::get<1>(typeSizeTupleOpt.value()) });
							else
								return false;
						}
						else if (key.compare("MinWidth") == 0)
						{
							if (std::optional<std::tuple<RowColumnType, float>> typeSizeTupleOpt = ParseRowColumnTypeAndSize(columnDefinition[key], layout))
								column.value()->MinWidth({ std::get<0>(typeSizeTupleOpt.value()), std::get<1>(typeSizeTupleOpt.value()) });
							else
								return false;
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
					UI_ERROR("Failed to add a column for layout with name: {}", layout->Name());
					UI_ERROR("Intended type: {} - Intended width: {}", rowColType, rowColSize);
					return false;
				}
			}
		}
		else
		{
			UI_ERROR("ColumnDefinitions value must be an array type. Invalid value: {}", data["ColumnDefinitions"]);
			return false;
		}
	}
	else
	{
		// Add a single column that spans the layout
		if (!layout->AddColumn({ RowColumnType::STAR, 1.0f }))
		{
			UI_ERROR("Failed to add a single default column for layout with name: {}", layout->Name());
			return false;
		}
	}

	return true;
}
bool UI::LoadSubLayout(Layout* parent, json& data, const std::string& name) noexcept
{
	// First, import any necessary data
	if (data.contains("import"))
	{
		if (!ImportJSON(data))
			return false;
	}

	// Parse the row/column/rowspan/columnspan values
	if (std::optional<RowColumnPosition> positionOpt = ParseRowColumnPosition(data))
	{
		if (std::optional<Layout*> sublayoutOpt = parent->AddSubLayout(positionOpt.value(), name))
			return LoadLayoutDetails(sublayoutOpt.value(), data);
	}

	UI_ERROR("Sublayout with name '{}': Failed to add as sublayout.", name);
	return false;
}

bool UI::ParseGlobalStyles(json& data) noexcept
{
	for (auto& [key, value] : data.items())
	{
		if (data[key].contains("Type"))
		{
			if (!data[key]["Type"].is_string())
			{
				UI_ERROR("Failed to parse global data for key '{}'. 'Type' field must be a string.\nInvalid value : {}", key, data[key]["Type"]);
				return false;
			}

			std::string styleType = data[key]["Type"].get<std::string>();

			// Check if key is a key for a style loader
			if (m_loadStyleFunctions.find(styleType) != m_loadStyleFunctions.end())
			{
				if (std::optional<std::shared_ptr<Style>> styleOpt = m_loadStyleFunctions.at(styleType)(m_deviceResources, data[key], key))
					m_globalJsonData->m_stylesMap[key] = styleOpt.value();
				else
				{
					UI_ERROR("Failed to parse global data for key '{}'. The call to the LoadStyle function for style '{}' failed.", key, styleType);
					return false;
				}
			}
			else if (styleType.size() > 5 && styleType.substr(styleType.size() - 5, 5).compare("Style") == 0) // If the typeString ends in "Style", warn the user they probably did not set up the StyleLoader correctly
			{
				EG_CORE_WARN("{}:{} - Found global style '{}' for key '{}', but no associated LoadStyle function. You may have forgotten to call UI::SetStyleLoaderFunction()", __FILE__, __LINE__, styleType, key);
			}
		}
	}

	return true;
}
std::optional<RowColumnPosition> UI::ParseRowColumnPosition(json& data) noexcept
{
	RowColumnPosition position;
	position.Row = 0;
	position.Column = 0;
	position.RowSpan = 1;
	position.ColumnSpan = 1;

	if (data.contains("Row"))
	{
		if (!data["Row"].is_number_unsigned())
		{
			UI_ERROR("'Row' value must be an unsigned int.\nInvalid value: {}", data["Row"]);
			return std::nullopt;
		}

		position.Row = data["Row"].get<unsigned int>();
	}

	if (data.contains("Column"))
	{
		if (!data["Column"].is_number_unsigned())
		{
			UI_ERROR("'Column' value must be an unsigned int.\nInvalid value: {}", data["Column"]);
			return std::nullopt;
		}

		position.Column = data["Column"].get<unsigned int>();
	}

	if (data.contains("RowSpan"))
	{
		if (!data["RowSpan"].is_number_unsigned())
		{
			UI_ERROR("'RowSpan' value must be an unsigned int.\nInvalid value : {}", data["RowSpan"]);
			return std::nullopt;
		}

		position.RowSpan = data["RowSpan"].get<unsigned int>();

		if (position.RowSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'RowSpan' with value of 0. Setting rowSpan = 1", __FILE__, __LINE__);
			position.RowSpan = 1;
		}
	}

	if (data.contains("ColumnSpan"))
	{
		if (!data["ColumnSpan"].is_number_unsigned())
		{
			UI_ERROR("{}", "'ColumnSpan' value must be an unsigned int.\nInvalid value : {}", data["ColumnSpan"]);
			return std::nullopt;
		}

		position.ColumnSpan = data["ColumnSpan"].get<unsigned int>();

		if (position.ColumnSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'ColumnSpan' with value of 0. Setting columnSpan = 1", __FILE__, __LINE__);
			position.ColumnSpan = 1;
		}
	}

	return position;
}
std::optional<std::tuple<RowColumnType, float>> UI::ParseRowColumnTypeAndSize(json& data, Layout* layout) noexcept
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
		if (size <= 0.0f)
		{
			UI_ERROR("Fixed Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layout->Name(), size);
			return std::nullopt;
		}
		return std::make_tuple(type, size);
	}
	else if (data.is_string())
	{
		std::string dataString = data.get<std::string>();
		if (dataString.size() == 0)
		{
			UI_ERROR("{}", "RowDefinition Height/Width value for layout '{}' must not be empty.", layout->Name());
			return std::nullopt;
		}

		try {
			// Don't need to take a substring of datastring to remove '%' or '*' because stof will read all valid
			// digits and stop once it hits the end of the string or a non-numeric character
			size = std::stof(dataString);
		}
		catch (const std::invalid_argument& e)
		{
			UI_ERROR("Invalid Height/Width value for layout ({}) could not be parsed to a float: {}.\nException Message: {}", layout->Name(), dataString, e.what());
			return std::nullopt;
		}
		catch (const std::out_of_range& e)
		{
			UI_ERROR("Invalid Height/Width value for layout ({}). Caught out of range exception for value '{}'.\nException Message: {}", layout->Name(), dataString, e.what());
			return std::nullopt;
		}

		// Get the type and perform some bounds checking
		switch (dataString.back())
		{
		case '%': 
			type = RowColumnType::PERCENT; 
			if (size < 0.0f)
			{
				UI_ERROR("Percent Height/Width value for layout '{}' cannot be less than 0.\nInvalid value: {}", layout->Name(), size);
				return std::nullopt;
			}
			else if (size > 100.0f)
			{
				UI_ERROR("Percent Height/Width value for layout '{}' cannot be greater than 100.\nInvalid value: {}", layout->Name(), size);
				return std::nullopt;
			}

			size /= 100.0f; // Must divide by 100 to stay within the range [0, 1]
			
			break;
		case '*': 
			type = RowColumnType::STAR; 
			if (size <= 0.0f)
			{
				UI_ERROR("Star Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layout->Name(), size);
				return std::nullopt;
			}
			break;
		default:
			type = RowColumnType::FIXED; 
			if (size <= 0.0f)
			{
				UI_ERROR("Fixed Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layout->Name(), size);
				return std::nullopt;
			}
			break;
		}

		return std::make_tuple(type, size);
	}

	UI_ERROR("Height/Width value must be either a number or a string.\nLayout Name: {}\nInvalid value: {}", layout->Name(), data);
	return std::nullopt;
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
		
		if (std::optional<json> jsonImportOpt = LoadJSONFile(importPath))
		{
			jsonImport = jsonImportOpt.value();
		}
		else
		{
			UI_ERROR("Failed to import json file: {}", importPath.string());
			return false;
		}
	}
	else
	{
		// 'import' value is a json key that should exist at the root level of the initial json object
		if (!m_jsonRoot.contains(importValue))
		{
			UI_ERROR("Unable to import key '{}'. Does not exist at the json root level.", importValue);
			return false;
		}

		jsonImport = m_jsonRoot[importValue];
	}

	// The imported json must be a json object
	if (!jsonImport.is_object())
	{
		UI_ERROR("Unable to import key '{}'. Imported json value must be a json object.\nInvalid value: {}", importValue, jsonImport);
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