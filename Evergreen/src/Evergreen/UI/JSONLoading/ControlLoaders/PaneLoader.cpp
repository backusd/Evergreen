#include "pch.h"
#include "PaneLoader.h"

namespace Evergreen
{
void PaneLoader::LogWarningForRowColumnPositionAndMarginKeywords(json& data)
{
	constexpr std::array keysToIgnore{ "Row", "Column", "RowSpan", "ColumnSpane", "Margin"};
	for (const std::string& s : keysToIgnore)
	{
		if (data.contains(s))
		{
			EG_CORE_WARN("Ignoring key '{}' in Pane with name '{}'. Row/Column Positioning and Margin keywords do not make sense for Panes", s, m_name);
			data.erase(s);
		}
	}
}

float PaneLoader::ParseTop(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Top"), "Pane control with name '{}': 'Top' key is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Top"].is_number(), "Pane control with name '{}': 'Top' field must be a number. Invalid Pane object: {}", m_name, data.dump(4));

	float top = data["Top"].get<float>();
	JSON_LOADER_EXCEPTION_IF_FALSE(top > 0.0f, "Pane control with name '{}': 'Top' value must be greater than 0.0f. Invalid Pane object: {}", m_name, data.dump(4));

	return top;
}
float PaneLoader::ParseLeft(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Left"), "Pane control with name '{}': 'Left' key is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Left"].is_number(), "Pane control with name '{}': 'Left' field must be a number. Invalid Pane object: {}", m_name, data.dump(4));

	float left = data["Left"].get<float>();
	JSON_LOADER_EXCEPTION_IF_FALSE(left > 0.0f, "Pane control with name '{}': 'Left' value must be greater than 0.0f. Invalid Pane object: {}", m_name, data.dump(4));

	return left;
}
float PaneLoader::ParseHeight(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Height"), "Pane control with name '{}': 'Height' key is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Height"].is_number(), "Pane control with name '{}': 'Height' field must be a number. Invalid Pane object: {}", m_name, data.dump(4));

	float height = data["Height"].get<float>();
	JSON_LOADER_EXCEPTION_IF_FALSE(height > 0.0f, "Pane control with name '{}': 'Height' value must be greater than 0.0f. Invalid Pane object: {}", m_name, data.dump(4));

	return height;
}
float PaneLoader::ParseWidth(json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Width"), "Pane control with name '{}': 'Width' key is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Width"].is_number(), "Pane control with name '{}': 'Width' field must be a number. Invalid Pane object: {}", m_name, data.dump(4));

	float width = data["Width"].get<float>();
	JSON_LOADER_EXCEPTION_IF_FALSE(width > 0.0f, "Pane control with name '{}': 'Width' value must be greater than 0.0f. Invalid Pane object: {}", m_name, data.dump(4));

	return width;
}

bool PaneLoader::ParseResizable(json& data)
{
	if (data.contains("Resizable"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Resizable"].is_boolean(), "Pane control with name '{}': 'Resizable' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		return data["Resizable"].get<bool>();
	}
	return true;
}
bool PaneLoader::ParseRelocatable(json& data)
{
	if (data.contains("Relocatable"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Relocatable"].is_boolean(), "Pane control with name '{}': 'Relocatable' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		return data["Relocatable"].get<bool>();
	}
	return true;
}

std::unique_ptr<ColorBrush> PaneLoader::ParseBackgroundBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (data.contains("BackgroundBrush"))
	{
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BackgroundBrush"]));
	}

	return nullptr;
}
std::unique_ptr<ColorBrush> PaneLoader::ParseBorderBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (data.contains("BorderBrush"))
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"]));

	// Note: we are allowed to return nullptr here because the Pane class will just create a default border brush
	return nullptr;
}
std::array<float, 4> PaneLoader::ParseBorderWidth(json& data)
{
	std::array<float, 4> borderWidths{ 0.0f, 0.0f, 0.0f, 0.0f };

	if (data.contains("BorderWidth"))
	{
		if (data["BorderWidth"].is_number())
		{
			float width = data["BorderWidth"].get<float>();
			JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "Pane control with name '{}': 'BorderWidth' is not allowed to be less than 0. Invalid Pane object: {}", m_name, data.dump(4));
			borderWidths.fill(width);
			return borderWidths;
		}

		if (data["BorderWidth"].is_array())
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].size() == 4, "Pane control with name '{}': When specificying 'BorderWidth' as an array of floats, there must be exactly 4 values in the array. Invalid Pane object: {}", m_name, data.dump(4));

			float width = 0.0f;
			for (unsigned int iii = 0; iii < 4; ++iii)
			{
				JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"][iii].is_number(), "Pane control with name '{}': When specificying 'BorderWidth' as an array, the array values must be parsable numbers. Invalid Pane object: {}", m_name, data.dump(4));
				width = data["BorderWidth"][iii].get<float>();
				JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "Pane control with name '{}': 'BorderWidth' array values are not allowed to be less than 0. Invalid Pane object: {}", m_name, data.dump(4));
				borderWidths[iii] = width;
			}
			return borderWidths;
		}

		JSON_LOADER_EXCEPTION("Pane control with name '{}': 'BorderWidth' value must either be a number of an array of 4 floats. Invalid Pane object: {}", m_name, data.dump(4));
	}

	return borderWidths;
}

bool PaneLoader::ParseIncludeTitleBar(json& data)
{
	if (data.contains("IncludeTitleBar"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["IncludeTitleBar"].is_boolean(), "Pane control with name '{}': 'IncludeTitleBar' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		return data["IncludeTitleBar"].get<bool>();
	}
	return true;
}
std::unique_ptr<ColorBrush> PaneLoader::ParseTitleBarBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (data.contains("TitleBarBrush"))
	{
		return std::move(JSONLoaders::LoadBrush(deviceResources, data["TitleBarBrush"]));
	}

	return nullptr;
}
float PaneLoader::ParseTitleBarHeight(json& data)
{
	if (data.contains("TitleBarHeight"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["TitleBarHeight"].is_number(), "Pane control with name '{}': 'TitleBarHeight' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));

		float height = data["TitleBarHeight"].get<float>();

		JSON_LOADER_EXCEPTION_IF_FALSE(height > 0.0f, "Pane control with name '{}': 'TitleBarHeight' must be greater than 0. Invalid Pane object: {}", m_name, data.dump(4));

		return height;
	}

	return 20.0f;
}

void PaneLoader::ParseBorderOffsets(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "pane is nullptr");

	if (data.contains("BorderTopLeftOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopLeftOffsetX"].is_number(), "Pane control with name '{}': 'BorderTopLeftOffsetX' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float value = data["BorderTopLeftOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Pane control with name '{}': 'BorderTopLeftOffsetX' value must be >= 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->BorderTopLeftOffsetX(value);
	}
	if (data.contains("BorderTopLeftOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopLeftOffsetY"].is_number(), "Pane control with name '{}': 'BorderTopLeftOffsetY' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float value = data["BorderTopLeftOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Pane control with name '{}': 'BorderTopLeftOffsetY' value must be >= 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->BorderTopLeftOffsetY(value);
	}
	if (data.contains("BorderTopRightOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopRightOffsetX"].is_number(), "Pane control with name '{}': 'BorderTopRightOffsetX' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float value = data["BorderTopRightOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Pane control with name '{}': 'BorderTopRightOffsetX' value must be >= 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->BorderTopRightOffsetX(value);
	}
	if (data.contains("BorderTopRightOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopRightOffsetY"].is_number(), "Pane control with name '{}': 'BorderTopRightOffsetY' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float value = data["BorderTopRightOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Pane control with name '{}': 'BorderTopRightOffsetY' value must be >= 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->BorderTopRightOffsetY(value);
	}
	if (data.contains("BorderBottomLeftOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomLeftOffsetX"].is_number(), "Pane control with name '{}': 'BorderBottomLeftOffsetX' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float value = data["BorderBottomLeftOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Pane control with name '{}': 'BorderBottomLeftOffsetX' value must be >= 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->BorderBottomLeftOffsetX(value);
	}
	if (data.contains("BorderBottomLeftOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomLeftOffsetY"].is_number(), "Pane control with name '{}': 'BorderBottomLeftOffsetY' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float value = data["BorderBottomLeftOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Pane control with name '{}': 'BorderBottomLeftOffsetY' value must be >= 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->BorderBottomLeftOffsetY(value);
	}
	if (data.contains("BorderBottomRightOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomRightOffsetX"].is_number(), "Pane control with name '{}': 'BorderBottomRightOffsetX' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float value = data["BorderBottomRightOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Pane control with name '{}': 'BorderBottomRightOffsetX' value must be >= 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->BorderBottomRightOffsetX(value);
	}
	if (data.contains("BorderBottomRightOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomRightOffsetY"].is_number(), "Pane control with name '{}': 'BorderBottomRightOffsetY' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float value = data["BorderBottomRightOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Pane control with name '{}': 'BorderBottomRightOffsetY' value must be >= 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->BorderBottomRightOffsetY(value);
	}
}

void PaneLoader::ParseCornerRadius(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	if (data.contains("CornerRadius"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("CornerRadiusX"), "Pane control with name '{}': When 'CornerRadius' field is used, it is invalid to also include 'CornerRadiusX'. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("CornerRadiusY"), "Pane control with name '{}': When 'CornerRadius' field is used, it is invalid to also include 'CornerRadiusY'. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadius"].is_number(), "Pane control with name '{}': 'CornerRadius' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float radius = data["CornerRadius"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radius >= 0.0f, "Pane control with name '{}': 'CornerRadius' must be greater than or equal to 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->SetCornerRadius(radius);
	}
	else if (data.contains("CornerRadiusX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("CornerRadiusY"), "Pane control with name '{}': When 'CornerRadiusX' field is used, 'CornerRadiusY' is also required. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadiusX"].is_number(), "Pane control with name '{}': 'CornerRadiusX' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["CornerRadiusY"].is_number(), "Pane control with name '{}': 'CornerRadiusY' value must be a number. Invalid Pane object: {}", m_name, data.dump(4));
		float radiusX = data["CornerRadiusX"].get<float>();
		float radiusY = data["CornerRadiusY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(radiusX >= 0.0f, "Pane control with name '{}': 'CornerRadiusX' must be greater than or equal to 0. Invalid Pane object: {}", m_name, data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(radiusY >= 0.0f, "Pane control with name '{}': 'CornerRadiusY' must be greater than or equal to 0. Invalid Pane object: {}", m_name, data.dump(4));
		pane->SetCornerRadius(radiusX, radiusY);
	}
	else
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("CornerRadiusY"), "Pane control with name '{}': When 'CornerRadiusY' field is used, 'CornerRadiusX' is also required. Invalid Pane object: {}", m_name, data.dump(4));
	}
}
void PaneLoader::ParseIsMinimized(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");
	if (data.contains("IsMinimized"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["IsMinimized"].is_boolean(), "Pane control with name '{}': 'IsMinimized' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		pane->SetMinimized(data["IsMinimized"].get<bool>());
	}
}
void PaneLoader::ParseIsVisible(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");
	if (data.contains("IsVisible"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["IsVisible"].is_boolean(), "Pane control with name '{}': 'IsVisible' field must be a boolean. Invalid Pane object: {}", m_name, data.dump(4));
		pane->SetVisible(data["IsVisible"].get<bool>());
	}
}
void PaneLoader::ParseTitle(Pane* pane, json& data)
{
	EG_CORE_ASSERT(pane != nullptr, "Pane cannot be nullptr");

	bool includeTitleBar = ParseIncludeTitleBar(data);
	if (!includeTitleBar)
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains("Title"), "Pane control with name '{}': When 'IncludeTitleBar' field is false, it is invalid to also include a 'Title' field. Invalid Pane object: {}", m_name, data.dump(4));
		return;
	}

	if (data.contains("Title"))
	{
		if (data["Title"].is_string())
		{
			pane->ClearTitleBarLayoutAndAddTitle(data["Title"].get<std::string>());
		}
		else
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data["Title"].is_object(), "Pane control with name '{}': 'Title' field must either be a string or a json object that represents a Layout. Invalid Pane object: {}", m_name, data.dump(4));
			JSONLoaders::LoadLayout(pane->GetDeviceResources(), pane->GetTitleBarLayout(), data["Title"]);
		}
	}
	else
	{
		// Just let the title be empty
		pane->ClearTitleBarLayoutAndAddTitle("");
	}
}
void PaneLoader::ParseContent(Pane* pane, json& data)
{
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Content"), "Pane control with name '{}': 'Content' field is required. Incomplete Pane object: {}", m_name, data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Content"].is_object(), "Pane control with name '{}': 'Content' field must be a json object that represents a Layout. Invalid Pane object: {}", m_name, data.dump(4));
	JSONLoaders::LoadLayout(pane->GetDeviceResources(), pane->GetContentLayout(), data["Content"]);
}

}