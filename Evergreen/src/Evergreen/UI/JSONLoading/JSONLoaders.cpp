#include "pch.h"
#include "JSONLoaders.h"
#include "Evergreen/UI/Utils/ColorHelper.h"
#include <fstream>

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

	// Stops
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

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "Type", "Stops", "Gamma", "ExtendMode", "Axis" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - JSONLoaders::LoadGradientBrush() unrecognized key: '{}'.", __FILE__, __LINE__, key);
	}

	return std::move(std::make_unique<GradientBrush>(deviceResources, stops, axis, extendMode, gamma));
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadRadialBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	std::vector<D2D1_GRADIENT_STOP> stops;
	D2D1_POINT_2F originOffset = D2D1::Point2F();
	D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP;
	D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2;

	// Stops
	if (!data.contains("Stops"))
	{
		EG_CORE_ERROR("{}:{} - RadialBrush json object must have key 'Stops'. Incomplete 'RadialBrush' object: {}", __FILE__, __LINE__, data.dump(4));
		return nullptr;
	}

	if (!data["Stops"].is_array())
	{
		EG_CORE_ERROR("{}:{} - RadialBrush json object 'Stops' value must be an array. Invalid 'RadialBrush' object: {}", __FILE__, __LINE__, data.dump(4));
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
				EG_CORE_WARN("{}:{} - JSONLoaders::LoadRadialBrush() 'Stops' value unrecognized key: '{}'.", __FILE__, __LINE__, key);
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

	// OriginOffset
	if (data.contains("OriginOffset"))
	{
		if (!data["OriginOffset"].is_array())
		{
			EG_CORE_ERROR("{}:{} - 'OriginOffset' value must be an array. Invalid 'OriginOffset' value: {}", __FILE__, __LINE__, data["OriginOffset"].dump(4));
			return nullptr;
		}

		if (data["OriginOffset"].size() != 2)
		{
			EG_CORE_ERROR("{}:{} - 'OriginOffset' array must have exactly 2 values. Invalid 'OriginOffset' value: {}", __FILE__, __LINE__, data["OriginOffset"].dump(4));
			return nullptr;
		}

		if (!data["OriginOffset"][0].is_number() || !data["OriginOffset"][1].is_number())
		{
			EG_CORE_ERROR("{}:{} - 'OriginOffset' array values must both be floats. Invalid 'OriginOffset' value: {}", __FILE__, __LINE__, data["OriginOffset"].dump(4));
			return nullptr;
		}

		originOffset = D2D1::Point2F(data["OriginOffset"][0].get<float>(), data["OriginOffset"][1].get<float>());
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
			EG_CORE_ERROR("{}:{} - RadialBrush 'ExtendMode' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, modeStr);
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
			EG_CORE_ERROR("{}:{} - RadialBrush 'Gamma' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, gammaStr);
			return nullptr;
		}

		gamma = gammaMap.at(gammaStr);
	}

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "Type", "Stops", "Gamma", "ExtendMode", "OriginOffset" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - JSONLoaders::LoadRadialBrush() 'Stops' unrecognized key: '{}'.", __FILE__, __LINE__, key);
	}

	return std::move(std::make_unique<RadialBrush>(deviceResources, stops, originOffset, extendMode, gamma));
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadBitmapBrush(std::shared_ptr<DeviceResources> deviceResources, const json& data) noexcept
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	std::wstring file;
	TRANSFORM_TO_RECT_METHOD method = TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_FILL_RECT;
	D2D1_BITMAP_BRUSH_PROPERTIES props = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);

	// File
	if (!data.contains("File"))
	{
		EG_CORE_ERROR("{}:{} - BitmapBrush json object must have key 'File'. Incomplete 'BitmapBrush' object: {}", __FILE__, __LINE__, data.dump(4));
		return nullptr;
	}

	if (!data["File"].is_string())
	{
		EG_CORE_ERROR("{}:{} - BitmapBrush json object 'File' value must be a string. Invalid 'BitmapBrush' object: {}", __FILE__, __LINE__, data.dump(4));
		return nullptr;
	}

	std::string fileStr = data["File"].get<std::string>();

	if (fileStr.size() == 0)
	{
		EG_CORE_ERROR("{}:{} - BitmapBrush json object 'File' value cannot be empty. Invalid 'BitmapBrush' object: {}", __FILE__, __LINE__, data.dump(4));
		return nullptr;
	}

	file = std::wstring(fileStr.begin(), fileStr.end());

	// TransformMethod
	if (data.contains("TransformMethod"))
	{
		if (!data["TransformMethod"].is_string())
		{
			EG_CORE_ERROR("{}:{} - BitmapBrush json object 'TransformMethod' value must be a string. Invalid 'BitmapBrush' object: {}", __FILE__, __LINE__, data.dump(4));
			return nullptr;
		}

		static const std::unordered_map<std::string, TRANSFORM_TO_RECT_METHOD> methodMap = {
			{ "KEEP_XY_RATIO_FILL_RECT", TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_FILL_RECT },
			{ "KEEP_XY_RATIO_UNDERFILL_RECT", TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_UNDERFILL_RECT },
			{ "SCALE_TO_EXACT_RECT_DIMENSIONS", TRANSFORM_TO_RECT_METHOD::SCALE_TO_EXACT_RECT_DIMENSIONS }
		};

		std::string methodStr = data["TransformMethod"].get<std::string>();

		if (methodMap.find(methodStr) == methodMap.end())
		{
			EG_CORE_ERROR("{}:{} - BitmapBrush 'TransformMethod' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, methodStr);
			return nullptr;
		}

		method = methodMap.at(methodStr);
	}

	// ExtendModeX
	if (data.contains("ExtendModeX"))
	{
		if (!data["ExtendModeX"].is_string())
		{
			EG_CORE_ERROR("{}:{} - BitmapBrush json object 'ExtendModeX' value must be a string. Invalid 'BitmapBrush' object: {}", __FILE__, __LINE__, data.dump(4));
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

		std::string modeStr = data["ExtendModeX"].get<std::string>();

		if (modeMap.find(modeStr) == modeMap.end())
		{
			EG_CORE_ERROR("{}:{} - BitmapBrush 'ExtendModeX' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, modeStr);
			return nullptr;
		}

		props.extendModeX = modeMap.at(modeStr);
	}

	// ExtendModeY
	if (data.contains("ExtendModeY"))
	{
		if (!data["ExtendModeY"].is_string())
		{
			EG_CORE_ERROR("{}:{} - BitmapBrush json object 'ExtendModeY' value must be a string. Invalid 'BitmapBrush' object: {}", __FILE__, __LINE__, data.dump(4));
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

		std::string modeStr = data["ExtendModeY"].get<std::string>();

		if (modeMap.find(modeStr) == modeMap.end())
		{
			EG_CORE_ERROR("{}:{} - BitmapBrush 'ExtendModeY' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, modeStr);
			return nullptr;
		}

		props.extendModeY = modeMap.at(modeStr);
	}

	// InterpolationMode
	if (data.contains("InterpolationMode"))
	{
		if (!data["InterpolationMode"].is_string())
		{
			EG_CORE_ERROR("{}:{} - BitmapBrush json object 'InterpolationMode' value must be a string. Invalid 'BitmapBrush' object: {}", __FILE__, __LINE__, data.dump(4));
			return nullptr;
		}

		static const std::unordered_map<std::string, D2D1_BITMAP_INTERPOLATION_MODE> modeMap = {
			{ "D2D1_INTERPOLATION_MODE_FORCE_DWORD", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_FORCE_DWORD },
			{ "D2D1_INTERPOLATION_MODE_LINEAR", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR },
			{ "D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR },
			{ "Linear", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR },
			{ "NearestNeighbor", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR },
		};

		std::string modeStr = data["InterpolationMode"].get<std::string>();

		if (modeMap.find(modeStr) == modeMap.end())
		{
			EG_CORE_ERROR("{}:{} - BitmapBrush 'InterpolationMode' field value was unrecognized. Invalid value: {}", __FILE__, __LINE__, modeStr);
			return nullptr;
		}

		props.interpolationMode = modeMap.at(modeStr);
	}

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "Type", "File", "TransformMethod", "ExtendModeX", "ExtendModeY", "InterpolationMode" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - JSONLoaders::LoadBitmapBrush() unrecognized key: '{}'.", __FILE__, __LINE__, key);
	}

	return std::move(std::make_unique<BitmapBrush>(deviceResources, file, method, props));
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

bool JSONLoaders::LoadUIImpl(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept
{
	// Store root directory as member variable so ImportJson can use it
	m_jsonRootDirectory = rootDirectory;
	std::filesystem::path rootFilePath = std::filesystem::path(rootDirectory).append(rootFile);

	if (std::optional<json> data = LoadJSONFile(rootFilePath))
	{
		// Make sure data has 'root' key
		if (!data.value().contains("root"))
		{
			EG_CORE_ERROR("{}:{} - File {} has no 'root' key", __FILE__, __LINE__, rootFilePath.string());
			return false;
		}

		// Set m_jsonRoot so we can access non-root keys for import within the Load methods
		m_jsonRoot = data.value();

		// Before constructing the layout, load global data that can be retrieved later on
		if (!LoadGlobalStyles(deviceResources))
		{
			EG_CORE_ERROR("{}:{} - Call to LoadGlobalStyles() failed", __FILE__, __LINE__);
			m_jsonRoot = {};
			return false;
		}

		// Load all the json data under the 'root' key
		if (!LoadLayoutDetails(deviceResources, rootLayout, m_jsonRoot["root"]))
		{
			m_jsonRoot = {};
			return false;
		}

		// There is a somewhat weird behavior in DirectX reporting memory leaks on application shutdown.
		// In a DEBUG build, DirectX will report on any DirectX resources that have outstanding reference
		// counts. However, this check appears to be performed prior to static class instance destruction.
		// This means that if we have a static class (ex. any singleton class) with a reference to DirectX 
		// resources (such as a shared_ptr to DeviceResources), DirectX will report a memory leak because
		// the static instance of the class has not yet been destructed.
		// 
		// In this specific case, JSONLoaders keeps a cache of all Styles that have been loaded and each
		// style has a shared_ptr to DeviceResources. So here, we just clean that up, which is also just a
		// good idea because there is no need to keep the cached styles around anyways.
		JSONLoaders::ClearCache();

		// LayoutCheck is entirely optional - In a Release build, this does nothing
		rootLayout->LayoutCheck();

		m_jsonRoot = {};
		return true;
	}

	EG_CORE_ERROR("{}:{} - Failed to load file '{}'", __FILE__, __LINE__, rootFilePath.string());
	m_jsonRoot = {};
	return false;
}
std::optional<json> JSONLoaders::LoadJSONFile(std::filesystem::path filePath) noexcept
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
			EG_CORE_ERROR("{}:{} - File: {}\njson::parse_error: {}", __FILE__, __LINE__, filePath.string(), e.what());
			return std::nullopt;
		}
	}

	EG_CORE_ERROR("{}:{} - Failed to open file: {}", __FILE__, __LINE__, filePath.string());
	return std::nullopt;
}
bool JSONLoaders::LoadGlobalStyles(std::shared_ptr<DeviceResources> deviceResources) noexcept
{
	for (auto& [key, value] : m_jsonRoot.items())
	{
		if (m_jsonRoot[key].contains("Type"))
		{
			if (!m_jsonRoot[key]["Type"].is_string())
			{
				EG_CORE_ERROR("{}:{} - Failed to parse global data for key '{}'. 'Type' field must be a string.\nInvalid value : {}", __FILE__, __LINE__, key, m_jsonRoot[key]["Type"].dump(4));
				return false;
			}

			std::string styleType = m_jsonRoot[key]["Type"].get<std::string>();

			if (JSONLoaders::IsStyleKey(styleType))
			{
				// Just calling LoadStyle is enough for the style to become cached within JSONLoaders.
				// Therefore, there is no reason to do anything with the return value.
				JSONLoaders::LoadStyle(deviceResources, styleType, m_jsonRoot[key], key);

			}
			else if (styleType.size() > 5 && styleType.substr(styleType.size() - 5, 5).compare("Style") == 0) // If the typeString ends in "Style", warn the user they probably did not set up the StyleLoader correctly
			{
				EG_CORE_WARN("{}:{} - Found global style '{}' for key '{}', but did not file a StyleLoader within JSONLoaders. You may have forgotten to call JSONLoaders::AddStyleLoader()", __FILE__, __LINE__, styleType, key);
			}
		}
	}

	return true;
}
bool JSONLoaders::ImportJSONImpl(json& data) noexcept
{
	if (!data.contains("import"))
	{
		EG_CORE_WARN("{}:{} - JSONLoaders::ImportJSON() called but could not find 'import' key: {}", __FILE__, __LINE__, data.dump(4));
		return true; // Return true because this is not a true error. The program can still continue to parse the json data.
	}

	if (!data["import"].is_string())
	{
		EG_CORE_ERROR("{}:{} - 'import' key must have a string-type value. Invalid value: {}", __FILE__, __LINE__, data["import"].dump(4));
		return false;
	}

	std::string importValue = data["import"].get<std::string>();

	if (importValue.size() == 0)
	{
		EG_CORE_WARN("{}:{} - 'import' key has an empty value. json data: {}", __FILE__, __LINE__, data.dump(4));
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
			EG_CORE_ERROR("{}:{} - Failed to import json file: {}", __FILE__, __LINE__, importPath.string());
			return false;
		}
	}
	else
	{
		// 'import' value is a json key that should exist at the root level of the initial json object
		if (!m_jsonRoot.contains(importValue))
		{
			EG_CORE_ERROR("{}:{} - Unable to import key '{}'. Does not exist at the json root level.", __FILE__, __LINE__, importValue);
			return false;
		}

		jsonImport = m_jsonRoot[importValue];
	}

	// The imported json must be a json object
	if (!jsonImport.is_object())
	{
		EG_CORE_ERROR("{}:{} - Unable to import key '{}'. Imported json value must be a json object.\nInvalid value: {}", __FILE__, __LINE__, importValue, jsonImport.dump(4));
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
			return false;
		}

		data[key] = jsonImport[key];
	}

	// Finally, remove the 'import' key because so the parsing code can't accidentally attempt to import it a second time
	data.erase("import");

	return true;
}

bool JSONLoaders::LoadLayoutDetails(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data) noexcept
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
			EG_CORE_ERROR("{}:{} - Control or sub-layout has no 'Type' definition: {}", __FILE__, __LINE__, data[key].dump(4));
			return false;
		}

		if (!data[key]["Type"].is_string())
		{
			EG_CORE_ERROR("{}:{} - Control or sub-layout 'Type' definition must be a string.\nInvalid value : {}", __FILE__, __LINE__, data[key]["Type"].dump(4));
			return false;
		}

		std::string type = data[key]["Type"].get<std::string>();

		// Load either a sublayout or control
		if (type.compare("Layout") == 0)
		{
			if (!LoadSubLayout(deviceResources, layout, data[key], key))
				return false;
		}
		else if (JSONLoaders::IsControlKey(type))
		{
			if (data[key].contains("import"))
			{
				if (!ImportJSON(data[key]))
					return false;
			}

			Control* control = JSONLoaders::LoadControl(deviceResources, type, layout, data[key], key);
			if (control == nullptr)
				EG_CORE_ERROR("Failed to load control with name '{}'.", key);
		}
		else
		{
			EG_CORE_WARN("Attempting to load control: {} (... not yet supported ...)", type);
		}
	}

	return true;
}

bool JSONLoaders::LoadLayoutRowDefinitions(Layout* layout, json& data) noexcept
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
					EG_CORE_ERROR("{}:{} - RowDefinition for layout '{}' does not contain 'Height' key: {}", __FILE__, __LINE__, layout->Name(), rowDefinition.dump(4));
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
								EG_CORE_ERROR("{}:{} - RowDefinition TopAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), rowDefinition[key].dump(4));
								return false;
							}
						}
						else if (key.compare("BottomAdjustable") == 0)
						{
							if (rowDefinition[key].is_boolean())
								row.value()->BottomIsAdjustable(rowDefinition[key].get<bool>());
							else
							{
								EG_CORE_ERROR("{}:{} - RowDefinition BottomAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), rowDefinition[key].dump(4));
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
					EG_CORE_ERROR("{}:{} - Failed to add a row for layout with name: {}", __FILE__, __LINE__, layout->Name());
					EG_CORE_ERROR("{}:{} - Intended type: {} - Intended height: {}", __FILE__, __LINE__, rowColType, rowColSize);
					return false;
				}
			}
		}
		else
		{
			EG_CORE_ERROR("{}:{} - RowDefinitions value must be an array type. Invalid value: {}", __FILE__, __LINE__, data["RowDefinitions"].dump(4));
			return false;
		}
	}
	else
	{
		// Add a single row that spans the layout
		if (!layout->AddRow({ RowColumnType::STAR, 1.0f }))
		{
			EG_CORE_ERROR("{}:{} - Failed to add a single default row for layout with name: {}", __FILE__, __LINE__, layout->Name());
			return false;
		}
	}

	return true;
}
bool JSONLoaders::LoadLayoutColumnDefinitions(Layout* layout, json& data) noexcept
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
					EG_CORE_ERROR("{}:{} - ColumnDefinition for layout '{}' does not contain 'Width' key: {}", __FILE__, __LINE__, layout->Name(), columnDefinition.dump(4));
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
								EG_CORE_ERROR("{}:{} - ColumnDefinition LeftAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), columnDefinition[key].dump(4));
								return false;
							}
						}
						else if (key.compare("RightAdjustable") == 0)
						{
							if (columnDefinition[key].is_boolean())
								column.value()->RightIsAdjustable(columnDefinition[key].get<bool>());
							else
							{
								EG_CORE_ERROR("{}:{} - ColumnDefinition RightAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), columnDefinition[key].dump(4));
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
					EG_CORE_ERROR("{}:{} - Failed to add a column for layout with name: {}", __FILE__, __LINE__, layout->Name());
					EG_CORE_ERROR("{}:{} - Intended type: {} - Intended width: {}", __FILE__, __LINE__, rowColType, rowColSize);
					return false;
				}
			}
		}
		else
		{
			EG_CORE_ERROR("{}:{} - ColumnDefinitions value must be an array type. Invalid value: {}", __FILE__, __LINE__, data["ColumnDefinitions"].dump(4));
			return false;
		}
	}
	else
	{
		// Add a single column that spans the layout
		if (!layout->AddColumn({ RowColumnType::STAR, 1.0f }))
		{
			EG_CORE_ERROR("{}:{} - Failed to add a single default column for layout with name: {}", __FILE__, __LINE__, layout->Name());
			return false;
		}
	}

	return true;
}
bool JSONLoaders::LoadSubLayout(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name) noexcept
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
			return LoadLayoutDetails(deviceResources, sublayoutOpt.value(), data);
	}

	EG_CORE_ERROR("{}:{} - Sublayout with name '{}': Failed to add as sublayout.", __FILE__, __LINE__, name);
	return false;
}

std::optional<RowColumnPosition> JSONLoaders::ParseRowColumnPosition(json& data) noexcept
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
			EG_CORE_ERROR("{}:{} - 'Row' value must be an unsigned int.\nInvalid value: {}", __FILE__, __LINE__, data["Row"].dump(4));
			return std::nullopt;
		}

		position.Row = data["Row"].get<unsigned int>();
	}

	if (data.contains("Column"))
	{
		if (!data["Column"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'Column' value must be an unsigned int.\nInvalid value: {}", __FILE__, __LINE__, data["Column"].dump(4));
			return std::nullopt;
		}

		position.Column = data["Column"].get<unsigned int>();
	}

	if (data.contains("RowSpan"))
	{
		if (!data["RowSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'RowSpan' value must be an unsigned int.\nInvalid value : {}", __FILE__, __LINE__, data["RowSpan"].dump(4));
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
			EG_CORE_ERROR("{}:{} - 'ColumnSpan' value must be an unsigned int.\nInvalid value : {}", __FILE__, __LINE__, data["ColumnSpan"].dump(4));
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
std::optional<std::tuple<RowColumnType, float>> JSONLoaders::ParseRowColumnTypeAndSize(json& data, Layout* layout) noexcept
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
			EG_CORE_ERROR("{}:{} - Fixed Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), size);
			return std::nullopt;
		}
		return std::make_tuple(type, size);
	}
	else if (data.is_string())
	{
		std::string dataString = data.get<std::string>();
		if (dataString.size() == 0)
		{
			EG_CORE_ERROR("{}:{} - RowDefinition Height/Width value for layout '{}' must not be empty.", __FILE__, __LINE__, layout->Name());
			return std::nullopt;
		}

		try {
			// Don't need to take a substring of datastring to remove '%' or '*' because stof will read all valid
			// digits and stop once it hits the end of the string or a non-numeric character
			size = std::stof(dataString);
		}
		catch (const std::invalid_argument& e)
		{
			EG_CORE_ERROR("{}:{} - Invalid Height/Width value for layout ({}) could not be parsed to a float: {}.\nException Message: {}", __FILE__, __LINE__, layout->Name(), dataString, e.what());
			return std::nullopt;
		}
		catch (const std::out_of_range& e)
		{
			EG_CORE_ERROR("{}:{} - Invalid Height/Width value for layout ({}). Caught out of range exception for value '{}'.\nException Message: {}", __FILE__, __LINE__, layout->Name(), dataString, e.what());
			return std::nullopt;
		}

		// Get the type and perform some bounds checking
		switch (dataString.back())
		{
		case '%':
			type = RowColumnType::PERCENT;
			if (size < 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Percent Height/Width value for layout '{}' cannot be less than 0.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				return std::nullopt;
			}
			else if (size > 100.0f)
			{
				EG_CORE_ERROR("{}:{} - Percent Height/Width value for layout '{}' cannot be greater than 100.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				return std::nullopt;
			}

			size /= 100.0f; // Must divide by 100 to stay within the range [0, 1]

			break;
		case '*':
			type = RowColumnType::STAR;
			if (size <= 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Star Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				return std::nullopt;
			}
			break;
		default:
			type = RowColumnType::FIXED;
			if (size <= 0.0f)
			{
				EG_CORE_ERROR("{}:{} - Fixed Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), size);
				return std::nullopt;
			}
			break;
		}

		return std::make_tuple(type, size);
	}

	EG_CORE_ERROR("{}:{} - Height/Width value must be either a number or a string.\nLayout Name: {}\nInvalid value: {}", __FILE__, __LINE__, layout->Name(), data.dump(4));
	return std::nullopt;
}

}