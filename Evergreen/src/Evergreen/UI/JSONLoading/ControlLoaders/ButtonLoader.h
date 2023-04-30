#pragma once
#include "pch.h"
#include "ControlLoader.h"
#include "Evergreen/UI/JSONLoading/JSONLoaders.h"
#include "Evergreen/UI/Controls/Button.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ButtonLoader : public ControlLoader
{
public:
	ButtonLoader(const ButtonLoader&) = delete;
	ButtonLoader& operator=(const ButtonLoader&) = delete;
	~ButtonLoader() noexcept override {}

	template<typename T>
	ND static inline Control* Load(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt) requires (std::is_base_of_v<Button, T>)
	{ 
		return Get().LoadImpl<T>(deviceResources, parent, data, name, rowColumnPositionOverride); 
	}

private:
	ButtonLoader() noexcept = default;

	static ButtonLoader& Get() noexcept
	{
		static ButtonLoader loader;
		return loader;
	}

	template<typename T>
	Control* LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<Button, T>);

	std::unique_ptr<ColorBrush> ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::unique_ptr<ColorBrush> ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data);
	std::array<float, 4> ParseBorderWidth(json& data);
	void ParseContent(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data);

	void ParseBorderOffsets(Button* button, json& data);
	void ParseCornerRadius(Button* button, json& data);
};
#pragma warning( pop )

template<typename T>
Control* ButtonLoader::LoadImpl(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride) requires (std::is_base_of_v<Button, T>)
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
	std::unique_ptr<ColorBrush> backgroundBrush = ParseBackgroundBrush(deviceResources, data); 
	EG_CORE_ASSERT(backgroundBrush != nullptr, "Not allowed to return nullptr. Should have thrown exception"); 

	//       Note: this function IS allowed to be nullptr and if so, the Button will just create a default brush
	std::unique_ptr<ColorBrush> borderBrush = ParseBorderBrush(deviceResources, data); 

	// Border Width
	std::array<float, 4> borderWidths = ParseBorderWidth(data); 

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "id", "Type", "Text", "Row", "Column", "RowSpan", "ColumnSpan", "Margin", 
	"BackgroundBrush", "BorderBrush", "BorderWidth", "Content", "BorderTopLeftOffsetX", "BorderTopLeftOffsetY",
	"BorderTopRightOffsetX", "BorderTopRightOffsetY", "BorderBottomLeftOffsetX", "BorderBottomLeftOffsetY",
	"BorderBottomRightOffsetX", "BorderBottomRightOffsetY", "CornerRadiusX", "CornerRadiusY", "CornerRadius" };
	for (auto& [key, value] : data.items()) 
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end()) 
			EG_CORE_WARN("{}:{} - Button control with name '{}'. Unrecognized key: '{}'.", __FILE__, __LINE__, m_name, key); 
	}

	// Create the new Button control
	T* button = parent->CreateControl<T>(rowCol, deviceResources, std::move(backgroundBrush), std::move(borderBrush), borderWidths, margin); 
	EG_CORE_ASSERT(button != nullptr, "Something went wrong, button is nullptr"); 

	button->Name(name); 
	button->ID(ParseID(data)); 

	ParseContent(deviceResources, button->GetLayout(), data); 
	ParseBorderOffsets(button, data); 
	ParseCornerRadius(button, data);

	return button;
}


}