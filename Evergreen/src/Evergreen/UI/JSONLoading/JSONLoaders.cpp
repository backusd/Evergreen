#include "pch.h"
#include "JSONLoaders.h"
#include "Evergreen/UI/Utils/ColorHelper.h"

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

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "Type", "Color" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - JSONLoaders::LoadSolidColorBrush() unrecognized key: '{}'.", __FILE__, __LINE__, key);
	}

	if (std::optional<D2D1_COLOR_F> colorOpt = JSONLoaders::LoadColor(data["Color"]))
		return std::move(std::make_unique<SolidColorBrush>(deviceResources, colorOpt.value()));
	
	EG_CORE_ERROR("{}:{} - JSONLoaders::LoadColor() failed for data: {}", __FILE__, __LINE__, data.dump(4));
	return nullptr;
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadGradientBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	std::vector<D2D1_GRADIENT_STOP> stops;
	GRADIENT_AXIS axis = GRADIENT_AXIS::VERTICAL;
	D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP;
	D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2;

	if (!data.contains("Stops"))
	{
		EG_CORE_ERROR("{}:{} - GradientBrush json object must have key 'Stops'. Incomplete 'GradientBrush' object: {}", __FILE__, __LINE__, data.dump(4));
		return nullptr;
	}

	if (!data["Stops"].is_array())
	{
		EG_CORE_ERROR("{}:{} - GradientBrush json object 'Stops' value must be an array. Invalid 'GradientBrush' object: {}", __FILE__, __LINE__, data.dump(4));
		return nullptr;
	}

	for (auto& stopObject : data["Stops"])
	{
		D2D1_GRADIENT_STOP stop;

		if (!stopObject.is_object())
		{
			EG_CORE_ERROR("{}:{} - 'Stops' array values must be json objects. Invalid 'Stops' object: {}", __FILE__, __LINE__, stopObject.dump(4));
			return nullptr;
		}

		if (!stopObject.contains("Color"))
		{
			EG_CORE_ERROR("{}:{} - Each 'Stops' array values must contain the key 'Color'. Invalid 'Stops' object: {}", __FILE__, __LINE__, stopObject.dump(4));
			return nullptr;
		}

		if (!stopObject.contains("Position"))
		{
			EG_CORE_ERROR("{}:{} - Each 'Stops' array values must contain the key 'Position'. Invalid 'Stops' object: {}", __FILE__, __LINE__, stopObject.dump(4));
			return nullptr;
		}

		// Warn about unrecognized keys
		constexpr std::array recognizedKeys{ "Color", "Position" };
		for (auto& [key, value] : stopObject.items())
		{
			if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
				EG_CORE_WARN("{}:{} - JSONLoaders::LoadGradientBrush() 'Stops' value unrecognized key: '{}'.", __FILE__, __LINE__, key);
		}

		if (std::optional<D2D1_COLOR_F> colorOpt = JSONLoaders::LoadColor(stopObject["Color"]))
			stop.color = colorOpt.value();
		else
		{
			EG_CORE_ERROR("{}:{} - 'Stops' array value failed to load color for object: {}", __FILE__, __LINE__, stopObject.dump(4));
			return nullptr;
		}

		if (!stopObject["Position"].is_number())
		{
			EG_CORE_ERROR("{}:{} - 'Stops' array value 'Position' field must be a number. Invalid 'Stops' object: {}", __FILE__, __LINE__, stopObject.dump(4));
			return nullptr;
		}

		stop.position = stopObject["Position"].get<float>();

		if (stop.position < 0.0f || stop.position > 1.0f)
		{
			EG_CORE_ERROR("{}:{} - 'Stops' array value 'Position' field must be between [0.0 - 1.0]. Invalid 'Stops' object: {}", __FILE__, __LINE__, stopObject.dump(4));
			return nullptr;
		}

		stops.push_back(stop);
	}

	// Axis
	if (data.contains("Axis"))
	{
		if (!data["Axis"].is_string())
		{
			EG_CORE_ERROR("{}:{} - 'Axis' value must be a string. Invalid 'Axis' value: {}", __FILE__, __LINE__, data.dump(4));
			return nullptr;
		}

		static const std::unordered_map<std::string, GRADIENT_AXIS> axisMap = {
			{ "Horizontal", GRADIENT_AXIS::HORIZONTAL },
			{ "Vertical", GRADIENT_AXIS::VERTICAL },
			{ "NESW", GRADIENT_AXIS::NESW },
			{ "NWSE", GRADIENT_AXIS::NWSE }
		};

		std::string axisStr = data["Axis"].get<std::string>();

		if (axisMap.find(axisStr) == axisMap.end())
		{
			EG_CORE_ERROR("{}:{} - GradientBrush 'Axis' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, axisStr);
			return nullptr;
		}

		axis = axisMap.at(axisStr);
	}

	// ExtendMode
	if (data.contains("ExtendMode"))
	{
		if (!data["ExtendMode"].is_string())
		{
			EG_CORE_ERROR("{}:{} - 'ExtendMode' value must be a string. Invalid 'ExtendMode' value: {}", __FILE__, __LINE__, data.dump(4));
			return nullptr;
		}

		static const std::unordered_map<std::string, D2D1_EXTEND_MODE> modeMap = {
			{ "D2D1_EXTEND_MODE_CLAMP", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP },
			{ "D2D1_EXTEND_MODE_MIRROR", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_MIRROR },
			{ "D2D1_EXTEND_MODE_WRAP", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_WRAP },
			{ "D2D1_EXTEND_MODE_FORCE_DWORD", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_FORCE_DWORD },
			{ "Clamp", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP },
			{ "Mirror", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_MIRROR },
			{ "Wrap", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_WRAP },
		};

		std::string modeStr = data["ExtendMode"].get<std::string>();

		if (modeMap.find(modeStr) == modeMap.end())
		{
			EG_CORE_ERROR("{}:{} - GradientBrush 'ExtendMode' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, modeStr);
			return nullptr;
		}

		extendMode = modeMap.at(modeStr);
	}

	// Gamma
	if (data.contains("Gamma"))
	{
		if (!data["Gamma"].is_string())
		{
			EG_CORE_ERROR("{}:{} - 'Gamma' value must be a string. Invalid 'Gamma' value: {}", __FILE__, __LINE__, data.dump(4));
			return nullptr;
		}

		static const std::unordered_map<std::string, D2D1_GAMMA> gammaMap = {
			{ "D2D1_GAMMA_1_0", D2D1_GAMMA::D2D1_GAMMA_1_0 },
			{ "D2D1_GAMMA_2_2", D2D1_GAMMA::D2D1_GAMMA_2_2 },
			{ "D2D1_GAMMA_FORCE_DWORD", D2D1_GAMMA::D2D1_GAMMA_FORCE_DWORD }
		};

		std::string gammaStr = data["Gamma"].get<std::string>();

		if (gammaMap.find(gammaStr) == gammaMap.end())
		{
			EG_CORE_ERROR("{}:{} - GradientBrush 'Gamma' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, gammaStr);
			return nullptr;
		}

		gamma = gammaMap.at(gammaStr);
	}

	return std::move(std::make_unique<GradientBrush>(deviceResources, stops, axis, extendMode, gamma));
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
	if (m_stylesCache.find(stylename) != m_stylesCache.end())
		return m_stylesCache[stylename];

	std::shared_ptr<Style> style = m_styleLoaders[key](deviceResources, data, stylename);

	if (style == nullptr)
		EG_CORE_ERROR("{}:{} - JSONLoaders: Style loader failed for stylename '{}' with key '{}' on data: {}", __FILE__, __LINE__, stylename, key, data.dump(4));
	else
		m_stylesCache[stylename] = style;

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