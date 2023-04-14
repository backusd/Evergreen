#include "pch.h"
#include "RectangleLoader.h"


namespace Evergreen
{
Control* RectangleLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride)
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

	// Parse Brushes
	std::unique_ptr<ColorBrush> brush = ParseBrush(deviceResources, data);

	// Parse Height & Width
	float height = ParseHeight(data);
	float width = ParseWidth(data);

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"Brush", "Height", "Width" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - Rectangle control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Rectangle control
	Rectangle* rectangle = parent->CreateControl<Rectangle>(rowCol, deviceResources, std::move(brush), height, width, margin);
	EG_CORE_ASSERT(rectangle != nullptr, "Something went wrong, rectangle is nullptr");

	rectangle->Name(name);
	rectangle->ID(ParseID(data));

	return rectangle;
}

std::unique_ptr<ColorBrush> RectangleLoader::ParseBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources"); 

	if (data.contains("Brush"))
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["Brush"]));

	return nullptr;
}
float RectangleLoader::ParseHeight(json& data)
{
	if (data.contains("Height"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Height"].is_number(), "Rectangle control with name '{}': 'Height' value must be a number. Invalid Rectangle object: {}", m_name, data.dump(4));
		float height = data["Height"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(height > 0.0f, "Rectangle control with name '{}': 'Height' value must be greater than 0. Invalid Rectangle object: {}", m_name, data.dump(4));
		return height;
	}

	return 10.0f;
}
float RectangleLoader::ParseWidth(json& data)
{
	if (data.contains("Width"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Width"].is_number(), "Rectangle control with name '{}': 'Width' value must be a number. Invalid Rectangle object: {}", m_name, data.dump(4));
		float width = data["Width"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(width > 0.0f, "Rectangle control with name '{}': 'Width' value must be greater than 0. Invalid Rectangle object: {}", m_name, data.dump(4));
		return width;
	}

	return 10.0f;
}

}