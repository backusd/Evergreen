#include "pch.h"
#include "JSONLoaders.h"
#include "Evergreen/UI/ColorHelper.h"

namespace Evergreen
{
// This method assumes it has been passed the value associated with the key 'Color'.
// Example: ... = JSONLoaders::LoadColor(data["Color"]);
// Color value in json can either be a string matching a D2D1::ColorF Enum value, or a list of 3 or 4 floats for RGB[A] values
std::optional<D2D1_COLOR_F> JSONLoaders::LoadColor(const json& data) noexcept
{
	if (data.is_string())
	{
		return Evergreen::ColorFromString(data.get<std::string>());
	}
	else if (data.is_array())
	{
		if (data.size() < 3 || data.size() > 4)
		{
			EG_CORE_ERROR("{}:{} - Color array must have exactly 3 or 4 float values. Invalid color array: {}", __FILE__, __LINE__, data.dump(4));
			return std::nullopt;
		}

		std::array<float, 4> rgba{ 1.0f, 1.0f, 1.0f, 1.0f };
		int iii = 0;
		for (auto& rgbaValue : data)
		{
			if (!rgbaValue.is_number_float())
			{
				EG_CORE_ERROR("{}:{} - Color array values must be floats. Invalid value: {}", __FILE__, __LINE__, rgbaValue.dump(4));
				return std::nullopt;
			}

			rgba[iii] = rgbaValue.get<float>();

			if (rgba[iii] < 0.0f || rgba[iii] > 1.0f)
			{
				EG_CORE_ERROR("{}:{} - Color array values must be between [0.0 - 1.0]. Invalid value: {}", __FILE__, __LINE__, rgba[iii]);
				return std::nullopt;
			}

			++iii;
		}

		return D2D1::ColorF(rgba[0], rgba[1], rgba[2], rgba[3]);
	}

	EG_CORE_ERROR("{}:{} - Invalid Color Value: {} - Must either be a string or array of RGB[A] values between 0.0 - 1.0", __FILE__, __LINE__, data.dump(4));
	return std::nullopt;
}

// This method assumes it has been passed the value associated with the key 'Brush'.
// Example: ... = JSONLoaders::LoadBrush(data["Brush"]);
std::unique_ptr<ColorBrush> JSONLoaders::LoadBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	
	// If the data is just a string or array, then we are to assume it is data for a SolidColorBrush
	if (data.is_string() || data.is_array())
	{
		if (std::optional<D2D1_COLOR_F> colorOpt = JSONLoaders::LoadColor(data))
			return std::move(std::make_unique<SolidColorBrush>(deviceResources, colorOpt.value()));

		EG_CORE_ERROR("{}:{} - JSONLoaders::LoadColor() failed for data: {}", __FILE__, __LINE__, data.dump(4));
		return nullptr;
	}
	else if (data.is_object())
	{
		if (!data.contains("Type"))
		{
			EG_CORE_ERROR("{}:{} - 'Brush' object value must contain the key 'Type'. Incomplete 'Brush' object: {}", __FILE__, __LINE__, data.dump(4));
			return nullptr;
		}
		if (!data["Type"].is_string())
		{
			EG_CORE_ERROR("{}:{} - 'Brush' object 'Type' value must be a string. Invalid value: {}", __FILE__, __LINE__, data["Type"].dump(4));
			return nullptr;
		}

		std::string type = data["Type"].get<std::string>();

		if (type.compare("SolidColorBrush") == 0)
			return std::move(JSONLoaders::LoadSolidColorBrush(deviceResources, data));

		if (type.compare("GradientBrush") == 0)
			return std::move(JSONLoaders::LoadGradientBrush(deviceResources, data));

		if (type.compare("RadialBrush") == 0)
			return std::move(JSONLoaders::LoadRadialBrush(deviceResources, data));

		if (type.compare("BitmapBrush") == 0)
			return std::move(JSONLoaders::LoadBitmapBrush(deviceResources, data));

		EG_CORE_ERROR("{}:{} - Unrecognized brush 'Type' value: {}", __FILE__, __LINE__, type);
		return nullptr;
	}
	
	EG_CORE_ERROR("{}:{} - 'Brush' value must either be a string, array of floats, or an object that specifies the Brush type. Invalid value: {}", __FILE__, __LINE__, data.dump(4));
	return nullptr;
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadSolidColorBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	if (!data.contains("Color"))
	{
		EG_CORE_ERROR("{}:{} - SolidColorBrush json object must have key 'Color'. Incomplete 'SolidColorBrush' object: {}", __FILE__, __LINE__, data.dump(4));
		return nullptr;
	}

	if (std::optional<D2D1_COLOR_F> colorOpt = JSONLoaders::LoadColor(data["Color"]))
		return std::move(std::make_unique<SolidColorBrush>(deviceResources, colorOpt.value()));
	
	EG_CORE_ERROR("{}:{} - JSONLoaders::LoadColor() failed for data: {}", __FILE__, __LINE__, data.dump(4));
	return nullptr;
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadGradientBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	return nullptr;
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadRadialBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	return nullptr;
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadBitmapBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	return nullptr;
}

std::shared_ptr<Style> JSONLoaders::LoadStyleImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, const json& data, const std::string& stylename) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// Check to see if name of the style already exists, otherwise, parse the json data
	if (m_existingStyles.find(stylename) != m_existingStyles.end())
		return m_existingStyles[stylename];

	std::shared_ptr<Style> style = m_styleLoaders[key](deviceResources, data, stylename);

	if (style == nullptr)
		EG_CORE_ERROR("{}:{} - JSONLoaders: Style loader failed for stylename '{}' with key '{}' on data: {}", __FILE__, __LINE__, stylename, key, data.dump(4));
	else
		m_existingStyles[stylename] = style;

	return style;
}

Control* JSONLoaders::LoadControlImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, const json& data, const std::string& name) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(parent != nullptr, "Parent layout should never be nullptr");

	if (m_controlLoaders.find(key) == m_controlLoaders.end())
	{
		EG_CORE_ERROR("{}:{} - JSONLoaders: Failed to load control with name '{}'. No control loader for key: {}", __FILE__, __LINE__, name, key);
		return nullptr;
	}

	return m_controlLoaders[key](deviceResources, parent, data, name);
}


}