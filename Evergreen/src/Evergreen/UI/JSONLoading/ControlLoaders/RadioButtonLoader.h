#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/RadioButton.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API RadioButtonLoader : public ControlLoader
{
public:
	RadioButtonLoader(const RadioButtonLoader&) = delete;
	RadioButtonLoader& operator=(const RadioButtonLoader&) = delete;
	~RadioButtonLoader() noexcept override {}

	template<typename T>
	ND inline static Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) requires (std::is_base_of_v<RadioButton, T>)
	{
		return Get().LoadImpl<T>(deviceResources, parent, data, name, rowColumnPositionOverride);
	}

private:
	RadioButtonLoader() noexcept = default;

	static RadioButtonLoader& Get() noexcept
	{
		static RadioButtonLoader loader;
		return loader;
	}

	template<typename T>
	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<RadioButton, T>);

	ND bool ParseIsChecked(json& data);
	ND float ParseInnerRadius(json& data);
	ND float ParseOuterRadius(json& data);
	ND std::unique_ptr<ColorBrush> ParseInnerBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	ND std::unique_ptr<ColorBrush> ParseOuterBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	
	void ParseOuterLineWidth(RadioButton* rb, json& data);

};
#pragma warning( pop )

template<typename T>
Control* RadioButtonLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<RadioButton, T>)
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

	// Parse IsChecked
	bool isChecked = ParseIsChecked(data);

	// Parse inner & outer radius
	float innerRadius = ParseInnerRadius(data);
	float outerRadius = ParseOuterRadius(data);
	JSON_LOADER_EXCEPTION_IF_FALSE(innerRadius < outerRadius, "RadioButton control with name '{}': 'InnerRadius' value must be less than 'OuterRadius' value. Invalid RadioButton object: {}", m_name, data.dump(4));

	// Parse Brushes
	std::unique_ptr<ColorBrush> innerBrush = std::move(ParseInnerBrush(deviceResources, data));
	std::unique_ptr<ColorBrush> outerBrush = std::move(ParseOuterBrush(deviceResources, data));

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Row", "Column", "RowSpan", "ColumnSpan", "Margin",
	"IsChecked", "InnerRadius", "OuterRadius", "InnerBrush", "OuterBrush", "OuterBrushLineWidth" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - RadioButton control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key);
	}

	// Create the new Text control
	T* rb = parent->CreateControl<T>(
		rowCol,
		deviceResources,
		isChecked,
		outerRadius,
		innerRadius,
		std::move(outerBrush),
		std::move(innerBrush),
		margin
	);
	EG_CORE_ASSERT(rb != nullptr, "Something went wrong, radiobutton is nullptr");

	rb->Name(name);
	rb->ID(ParseID(data));

	ParseOuterLineWidth(rb, data);

	return rb;
}
}