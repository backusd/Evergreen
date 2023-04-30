#include "pch.h"
#include "JSONLoaders.h"
#include "Evergreen/UI/Utils/ColorHelper.h"
#include <fstream>

namespace Evergreen
{
// This method assumes it has been passed the value associated with the key 'Color'.
// Example: ... = JSONLoaders::LoadColor(data["Color"]);
// Color value in json can either be a string matching a D2D1::ColorF Enum value, or a list of 3 or 4 floats for RGB[A] values
D2D1_COLOR_F JSONLoaders::LoadColor(json& data)
{
	if (data.is_string())
	{
		return Evergreen::ColorFromString(data.get<std::string>());
	}
	else if (data.is_array())
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.size() == 3 || data.size() == 4, "Color array must have exactly 3 or 4 float values. Invalid color array: {}", data.dump(4));

		std::array<float, 4> rgba{ 1.0f, 1.0f, 1.0f, 1.0f };
		int iii = 0;
		for (auto& rgbaValue : data)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(rgbaValue.is_number_float(), "Color array values must be floats. Invalid value: {}", rgbaValue.dump(4));

			rgba[iii] = rgbaValue.get<float>();

			JSON_LOADER_EXCEPTION_IF_FALSE(rgba[iii] >= 0.0f && rgba[iii] <= 1.0f, "Color array values must be between [0.0 - 1.0]. Invalid value: {}", rgba[iii]);

			++iii;
		}

		return D2D1::ColorF(rgba[0], rgba[1], rgba[2], rgba[3]);
	}

	JSON_LOADER_EXCEPTION("Invalid Color Value: {} - Must either be a string or array of RGB[A] values between 0.0 - 1.0", data.dump(4));
}

// This method assumes it has been passed the value associated with the key 'Brush'.
// Example: ... = JSONLoaders::LoadBrush(data["Brush"]);
std::unique_ptr<ColorBrush> JSONLoaders::LoadBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	
	// If the data is just a string or array, then we are to assume it is data for a SolidColorBrush
	if (data.is_string() || data.is_array())
	{
		D2D1_COLOR_F color = JSONLoaders::LoadColor(data);
		return std::move(std::make_unique<SolidColorBrush>(deviceResources, color));
	}
	else if (data.is_object())
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Type"), "'Brush' object value must contain the key 'Type'. Incomplete 'Brush' object: {}", data.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Type"].is_string(), "'Brush' object 'Type' value must be a string. Invalid value: {}", data["Type"].dump(4));

		std::string type = data["Type"].get<std::string>();

		if (type.compare("SolidColorBrush") == 0)
			return std::move(JSONLoaders::LoadSolidColorBrush(deviceResources, data));

		if (type.compare("GradientBrush") == 0)
			return std::move(JSONLoaders::LoadGradientBrush(deviceResources, data));

		if (type.compare("RadialBrush") == 0)
			return std::move(JSONLoaders::LoadRadialBrush(deviceResources, data));

		if (type.compare("BitmapBrush") == 0)
			return std::move(JSONLoaders::LoadBitmapBrush(deviceResources, data));

		JSON_LOADER_EXCEPTION("Unrecognized brush 'Type' value: {}", type);
	}
	
	JSON_LOADER_EXCEPTION("'Brush' value must either be a string, array of floats, or an object that specifies the Brush type. Invalid value: {}", data.dump(4));
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadSolidColorBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Color"), "SolidColorBrush json object must have key 'Color'. Incomplete 'SolidColorBrush' object: {}", data.dump(4));

	// Warn about unrecognized keys
	constexpr std::array recognizedKeys{ "Type", "Color" };
	for (auto& [key, value] : data.items())
	{
		if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
			EG_CORE_WARN("{}:{} - JSONLoaders::LoadSolidColorBrush() unrecognized key: '{}'.", __FILE__, __LINE__, key);
	}

	return std::move(std::make_unique<SolidColorBrush>(deviceResources, JSONLoaders::LoadColor(data["Color"])));
}
std::unique_ptr<ColorBrush> JSONLoaders::LoadGradientBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	std::vector<D2D1_GRADIENT_STOP> stops;
	GRADIENT_AXIS axis = GRADIENT_AXIS::VERTICAL;
	D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP;
	D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2;

	// Stops
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Stops"), "GradientBrush json object must have key 'Stops'. Incomplete 'GradientBrush' object: {}", data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Stops"].is_array(), "GradientBrush json object 'Stops' value must be an array. Invalid 'GradientBrush' object: {}", data.dump(4));

	for (auto& stopObject : data["Stops"])
	{
		D2D1_GRADIENT_STOP stop{};

		JSON_LOADER_EXCEPTION_IF_FALSE(stopObject.is_object(), "'Stops' array values must be json objects. Invalid 'Stops' object: {}", stopObject.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(stopObject.contains("Color"), "Each 'Stops' array values must contain the key 'Color'. Invalid 'Stops' object: {}", stopObject.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(stopObject.contains("Position"), "Each 'Stops' array values must contain the key 'Position'. Invalid 'Stops' object: {}", stopObject.dump(4));

		// Warn about unrecognized keys
		constexpr std::array recognizedKeys{ "Color", "Position" };
		for (auto& [key, value] : stopObject.items())
		{
			if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
				EG_CORE_WARN("{}:{} - JSONLoaders::LoadGradientBrush() 'Stops' value unrecognized key: '{}'.", __FILE__, __LINE__, key);
		}

		// Color
		stop.color = JSONLoaders::LoadColor(stopObject["Color"]);

		// Position
		JSON_LOADER_EXCEPTION_IF_FALSE(stopObject["Position"].is_number(), "'Stops' array value 'Position' field must be a number. Invalid 'Stops' object: {}", stopObject.dump(4));
		stop.position = stopObject["Position"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(stop.position >= 0.0f && stop.position <= 1.0f, "'Stops' array value 'Position' field must be between [0.0 - 1.0]. Invalid 'Stops' object: {}", stopObject.dump(4));

		stops.push_back(stop);
	}

	// Axis
	if (data.contains("Axis"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Axis"].is_string(), "'Axis' value must be a string. Invalid 'Axis' value: {}", data.dump(4));

		static const std::unordered_map<std::string, GRADIENT_AXIS> axisMap = {
			{ "Horizontal", GRADIENT_AXIS::HORIZONTAL },
			{ "Vertical", GRADIENT_AXIS::VERTICAL },
			{ "NESW", GRADIENT_AXIS::NESW },
			{ "NWSE", GRADIENT_AXIS::NWSE }
		};

		std::string axisStr = data["Axis"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(axisMap.find(axisStr) != axisMap.end(), "GradientBrush 'Axis' field value was unrecognized. Invalid value: {}", axisStr);

		axis = axisMap.at(axisStr);
	}

	// ExtendMode
	if (data.contains("ExtendMode"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ExtendMode"].is_string(), "'ExtendMode' value must be a string. Invalid 'ExtendMode' value: {}", data.dump(4));

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

		JSON_LOADER_EXCEPTION_IF_FALSE(modeMap.find(modeStr) != modeMap.end(), "GradientBrush 'ExtendMode' field value was unrecognized. Invalid value: {}", modeStr);

		extendMode = modeMap.at(modeStr);
	}

	// Gamma
	if (data.contains("Gamma"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Gamma"].is_string(), "'Gamma' value must be a string. Invalid 'Gamma' value: {}", data.dump(4));

		static const std::unordered_map<std::string, D2D1_GAMMA> gammaMap = {
			{ "D2D1_GAMMA_1_0", D2D1_GAMMA::D2D1_GAMMA_1_0 },
			{ "D2D1_GAMMA_2_2", D2D1_GAMMA::D2D1_GAMMA_2_2 },
			{ "D2D1_GAMMA_FORCE_DWORD", D2D1_GAMMA::D2D1_GAMMA_FORCE_DWORD }
		};

		std::string gammaStr = data["Gamma"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(gammaMap.find(gammaStr) != gammaMap.end(), "GradientBrush 'Gamma' field value was unrecognized. Invalid value: {}", gammaStr);

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
std::unique_ptr<ColorBrush> JSONLoaders::LoadRadialBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	std::vector<D2D1_GRADIENT_STOP> stops;
	D2D1_POINT_2F originOffset = D2D1::Point2F();
	D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP;
	D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2;

	// Stops
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("Stops"), "RadialBrush json object must have key 'Stops'. Incomplete 'RadialBrush' object: {}", data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["Stops"].is_array(), "RadialBrush json object 'Stops' value must be an array. Invalid 'RadialBrush' object: {}", data.dump(4));

	for (auto& stopObject : data["Stops"])
	{
		D2D1_GRADIENT_STOP stop{};

		JSON_LOADER_EXCEPTION_IF_FALSE(stopObject.is_object(), "'Stops' array values must be json objects. Invalid 'Stops' object: {}", stopObject.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(stopObject.contains("Color"), "Each 'Stops' array values must contain the key 'Color'. Invalid 'Stops' object: {}", stopObject.dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(stopObject.contains("Position"), "Each 'Stops' array values must contain the key 'Position'. Invalid 'Stops' object: {}", stopObject.dump(4));

		// Warn about unrecognized keys
		constexpr std::array recognizedKeys{ "Color", "Position" };
		for (auto& [key, value] : stopObject.items())
		{
			if (std::find(recognizedKeys.begin(), recognizedKeys.end(), key) == recognizedKeys.end())
				EG_CORE_WARN("{}:{} - JSONLoaders::LoadRadialBrush() 'Stops' value unrecognized key: '{}'.", __FILE__, __LINE__, key);
		}

		// Color
		stop.color = JSONLoaders::LoadColor(stopObject["Color"]);

		// Position
		JSON_LOADER_EXCEPTION_IF_FALSE(stopObject["Position"].is_number(), "'Stops' array value 'Position' field must be a number. Invalid 'Stops' object: {}", stopObject.dump(4));
		stop.position = stopObject["Position"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(stop.position >= 0.0f && stop.position <= 1.0f, "'Stops' array value 'Position' field must be between [0.0 - 1.0]. Invalid 'Stops' object: {}", stopObject.dump(4));

		stops.push_back(stop);
	}

	// OriginOffset
	if (data.contains("OriginOffset"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OriginOffset"].is_array(), "'OriginOffset' value must be an array. Invalid 'OriginOffset' value: {}", data["OriginOffset"].dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OriginOffset"].size() == 2, "'OriginOffset' array must have exactly 2 values. Invalid 'OriginOffset' value: {}", data["OriginOffset"].dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OriginOffset"][0].is_number() && data["OriginOffset"][1].is_number(), "'OriginOffset' array values must both be floats. Invalid 'OriginOffset' value: {}", data["OriginOffset"].dump(4));

		originOffset = D2D1::Point2F(data["OriginOffset"][0].get<float>(), data["OriginOffset"][1].get<float>());
	}

	// ExtendMode
	if (data.contains("ExtendMode"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ExtendMode"].is_string(), "'ExtendMode' value must be a string. Invalid 'ExtendMode' value: {}", data.dump(4));

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

		JSON_LOADER_EXCEPTION_IF_FALSE(modeMap.find(modeStr) != modeMap.end(), "RadialBrush 'ExtendMode' field value was unrecognized. Invalid value: {}", modeStr);

		extendMode = modeMap.at(modeStr);
	}

	// Gamma
	if (data.contains("Gamma"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Gamma"].is_string(), "'Gamma' value must be a string. Invalid 'Gamma' value: {}", data.dump(4));

		static const std::unordered_map<std::string, D2D1_GAMMA> gammaMap = {
			{ "D2D1_GAMMA_1_0", D2D1_GAMMA::D2D1_GAMMA_1_0 },
			{ "D2D1_GAMMA_2_2", D2D1_GAMMA::D2D1_GAMMA_2_2 },
			{ "D2D1_GAMMA_FORCE_DWORD", D2D1_GAMMA::D2D1_GAMMA_FORCE_DWORD }
		};

		std::string gammaStr = data["Gamma"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(gammaMap.find(gammaStr) != gammaMap.end(), "RadialBrush 'Gamma' field value was unrecognized. Invalid value: {}", gammaStr);

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
std::unique_ptr<ColorBrush> JSONLoaders::LoadBitmapBrush(std::shared_ptr<DeviceResources> deviceResources, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	std::wstring file;
	TRANSFORM_TO_RECT_METHOD method = TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_FILL_RECT;
	D2D1_BITMAP_BRUSH_PROPERTIES props = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);

	// File
	JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("File"), "BitmapBrush json object must have key 'File'. Incomplete 'BitmapBrush' object: {}", data.dump(4));
	JSON_LOADER_EXCEPTION_IF_FALSE(data["File"].is_string(), "BitmapBrush json object 'File' value must be a string. Invalid 'BitmapBrush' object: {}", data.dump(4));

	std::string fileStr = data["File"].get<std::string>();

	JSON_LOADER_EXCEPTION_IF_FALSE(fileStr.size() > 0, "BitmapBrush json object 'File' value cannot be empty. Invalid 'BitmapBrush' object: {}", data.dump(4));

	file = std::wstring(fileStr.begin(), fileStr.end());

	// TransformMethod
	if (data.contains("TransformMethod"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["TransformMethod"].is_string(), "BitmapBrush json object 'TransformMethod' value must be a string. Invalid 'BitmapBrush' object: {}", data.dump(4));

		static const std::unordered_map<std::string, TRANSFORM_TO_RECT_METHOD> methodMap = {
			{ "KEEP_XY_RATIO_FILL_RECT", TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_FILL_RECT },
			{ "KEEP_XY_RATIO_UNDERFILL_RECT", TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_UNDERFILL_RECT },
			{ "SCALE_TO_EXACT_RECT_DIMENSIONS", TRANSFORM_TO_RECT_METHOD::SCALE_TO_EXACT_RECT_DIMENSIONS }
		};

		std::string methodStr = data["TransformMethod"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(methodMap.find(methodStr) != methodMap.end(), "BitmapBrush 'TransformMethod' field value was unrecognized. Invalid value: {}", methodStr);

		method = methodMap.at(methodStr);
	}



	static const std::unordered_map<std::string, D2D1_EXTEND_MODE> extendModeMap = {
		{ "D2D1_EXTEND_MODE_CLAMP", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP },
		{ "D2D1_EXTEND_MODE_MIRROR", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_MIRROR },
		{ "D2D1_EXTEND_MODE_WRAP", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_WRAP },
		{ "D2D1_EXTEND_MODE_FORCE_DWORD", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_FORCE_DWORD },
		{ "Clamp", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP },
		{ "Mirror", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_MIRROR },
		{ "Wrap", D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_WRAP },
	};

	// ExtendModeX
	if (data.contains("ExtendModeX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ExtendModeX"].is_string(), "BitmapBrush json object 'ExtendModeX' value must be a string. Invalid 'BitmapBrush' object: {}", data.dump(4));

		std::string modeXStr = data["ExtendModeX"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(extendModeMap.find(modeXStr) != extendModeMap.end(), "BitmapBrush 'ExtendModeX' field value was unrecognized. Invalid value: {}", modeXStr);

		props.extendModeX = extendModeMap.at(modeXStr);
	}

	// ExtendModeY
	if (data.contains("ExtendModeY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ExtendModeY"].is_string(), "BitmapBrush json object 'ExtendModeY' value must be a string. Invalid 'BitmapBrush' object: {}", data.dump(4));

		std::string modeYStr = data["ExtendModeY"].get<std::string>();
		
		JSON_LOADER_EXCEPTION_IF_FALSE(extendModeMap.find(modeYStr) != extendModeMap.end(), "BitmapBrush 'ExtendModeY' field value was unrecognized. Invalid value: {}", modeYStr);

		props.extendModeY = extendModeMap.at(modeYStr);
	}

	// InterpolationMode
	if (data.contains("InterpolationMode"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["InterpolationMode"].is_string(), "BitmapBrush json object 'InterpolationMode' value must be a string. Invalid 'BitmapBrush' object: {}", data.dump(4));

		static const std::unordered_map<std::string, D2D1_BITMAP_INTERPOLATION_MODE> modeMap = {
			{ "D2D1_INTERPOLATION_MODE_FORCE_DWORD", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_FORCE_DWORD },
			{ "D2D1_INTERPOLATION_MODE_LINEAR", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR },
			{ "D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR },
			{ "Linear", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR },
			{ "NearestNeighbor", D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR },
		};

		std::string modeStr = data["InterpolationMode"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(modeMap.find(modeStr) != modeMap.end(), "BitmapBrush 'InterpolationMode' field value was unrecognized. Invalid value: {}", modeStr);

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

std::unique_ptr<Style> JSONLoaders::LoadStyleImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, json& data, const std::string& stylename)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");

	// Check to see if name of the style already exists, otherwise, parse the json data

	if (m_stylesCache.find(stylename) != m_stylesCache.end())
		return std::move(m_stylesCache[stylename]->Duplicate());

	std::unique_ptr<Style> style = m_styleLoaders[key](deviceResources, data, stylename);

	m_stylesCache[stylename] = std::move(style->Duplicate());

	return style;
}
Control* JSONLoaders::LoadControlImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& key, Layout* parent, json& data, const std::string& name, std::optional<RowColumnPosition> rowColumnPositionOverride)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(parent != nullptr, "Parent layout should never be nullptr");

	JSON_LOADER_EXCEPTION_IF_FALSE(m_controlLoaders.find(key) != m_controlLoaders.end(), "JSONLoaders: Failed to load control with name '{}'. No control loader for key: {}", name, key);

	return m_controlLoaders[key](deviceResources, parent, data, name, rowColumnPositionOverride);
}
void JSONLoaders::LoadLayoutImpl(std::shared_ptr<DeviceResources> deviceResources, const std::string& type, Layout* layout, json& data, const std::string& name)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources"); 
	EG_CORE_ASSERT(layout != nullptr, "Layout should never be nullptr");

	JSON_LOADER_EXCEPTION_IF_FALSE(m_layoutLoaders.find(type) != m_layoutLoaders.end(), "JSONLoaders: Failed to load layout with name '{}'. No layout loader for type: {}", name, type);

	m_layoutLoaders[type](deviceResources, layout, data, name);
}

bool JSONLoaders::LoadUIImpl(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept
{
	// So that we don't have to worry about removing names of controls when a control gets deleted,
	// its going to be best to just enforce the uniqueness of control names at the time we are loading
	// new controls. Therefore, clear the names cache and just enforce uniqueness of names for the controls
	// we are about to load here
	m_controlNames.clear();

	try
	{
		// Store root directory as member variable so ImportJson can use it
		m_jsonRootDirectory = rootDirectory;
		std::filesystem::path rootFilePath = std::filesystem::path(rootDirectory).append(rootFile);

		m_jsonRoot = LoadJSONFile(rootFilePath);

		// Before constructing the layout, load global data that can be retrieved later on
		LoadGlobalStyles(deviceResources);

		// Load all the json data under the 'root' key
		JSON_LOADER_EXCEPTION_IF_FALSE(m_jsonRoot.contains("root"), "{}", "JSONLoaders: Failed to load root layout. No 'root' key found");
		JSON_LOADER_EXCEPTION_IF_FALSE(m_jsonRoot["root"].contains("Type"), "{}", "JSONLoaders: Failed to load root layout. Root Layout MUST contain 'Type' key.");
		JSON_LOADER_EXCEPTION_IF_FALSE(m_jsonRoot["root"]["Type"].is_string(), "{}", "JSONLoaders: Failed to load root layout. Root Layout 'Type' value must be a string.");
		LoadLayoutImpl(deviceResources, m_jsonRoot["root"]["Type"].get<std::string>(), rootLayout, m_jsonRoot["root"], "root");

		// Finally, load all panes
		LoadPanes(deviceResources, rootLayout);

		// LayoutCheck is entirely optional - In a Release build, this does nothing
		rootLayout->LayoutCheck();

		// Cleanup
		m_jsonRoot = {};

		return true;
	}
	catch (const JSONLoadersException& ex)
	{
		EG_CORE_ERROR("Failed to load UI file '{}'", rootFile);
		EG_CORE_ERROR("Caught JSONLoadersException with message:\n{}", ex.what());
	}
	catch (const BaseException& ex)
	{
		EG_CORE_ERROR("Failed to load UI file '{}'", rootFile);
		EG_CORE_ERROR("Caught BaseException with message:\n{}", ex.what());
	}
	catch (json::parse_error& e)
	{
		EG_CORE_ERROR("Failed to load UI file '{}'", rootFile);
		EG_CORE_ERROR("Caught json::parse_error:\n{}", e.what());
	}
	catch (const std::exception& ex)
	{
		EG_CORE_ERROR("Failed to load UI file '{}'", rootFile);
		EG_CORE_ERROR("Caught std::exception with message:\n{}", ex.what());
	}
	catch (...)
	{
		EG_CORE_ERROR("Failed to load UI file '{}'", rootFile);
		EG_CORE_ERROR("{}", "Caught unidentified exception");
	}

	m_controlNames.clear();
	m_jsonRoot = {};
	return false;
}
void JSONLoaders::LoadControlsFromFileImpl(const std::string& fileName, Layout* parentLayout, std::optional<RowColumnPosition> rowColumnPositionOverride)
{
	// So that we don't have to worry about removing names of controls when a control gets deleted,
	// its going to be best to just enforce the uniqueness of control names at the time we are loading
	// new controls. Therefore, clear the names cache and just enforce uniqueness of names for the controls
	// we are about to load here
	m_controlNames.clear();

	try
	{
		std::filesystem::path filePath = std::filesystem::path(m_jsonRootDirectory).append(fileName);
		json data = LoadJSONFile(filePath);

		for (auto& [key, value] : data.items())
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data[key].contains("Type"), "JSON File: '{}' - Control with name '{}' has no 'Type' definition: {}", fileName, key, data[key].dump(4)); 
			JSON_LOADER_EXCEPTION_IF_FALSE(data[key]["Type"].is_string(), "JSON File: '{}' - Control with name '{}' has 'Type' definition that is not a string.\nInvalid value : {}", fileName, key, data[key]["Type"].dump(4));

			std::string type = data[key]["Type"].get<std::string>(); 

			// This function assumes that the Layout has already been created and we simply want to load a new
			// control. Therefore, we force the keys in the file to be Control keys
			if (JSONLoaders::IsControlKey(type))
			{
				Control* control = JSONLoaders::LoadControl(parentLayout->GetDeviceResources(), type, parentLayout, data[key], key, rowColumnPositionOverride);
				if (control == nullptr) 
					EG_CORE_ERROR("JSON File: '{}' - Failed to load control with name '{}'.", fileName, key); 
			}
			else
			{
				EG_CORE_WARN("JSON File: '{}' - Attempting to load control: {} (... not yet supported ...)", fileName, type);
			}
		}

		// LayoutCheck is entirely optional - In a Release build, this does nothing
		parentLayout->LayoutCheck();
	}
	catch (const JSONLoadersException& ex)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("Caught JSONLoadersException with message:\n{}", ex.what());
	}
	catch (const BaseException& ex)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("Caught BaseException with message:\n{}", ex.what());
	}
	catch (json::parse_error& e)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("Caught json::parse_error:\n{}", e.what());
	}
	catch (const std::exception& ex)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("Caught std::exception with message:\n{}", ex.what());
	}
	catch (...)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("{}", "Caught unidentified exception");
	}
}
void JSONLoaders::LoadLayoutFromFileImpl(const std::string& fileName, Layout* layoutToFill)
{
	// So that we don't have to worry about removing names of controls when a control gets deleted,
	// its going to be best to just enforce the uniqueness of control names at the time we are loading
	// new controls. Therefore, clear the names cache and just enforce uniqueness of names for the controls
	// we are about to load here
	m_controlNames.clear();

	try
	{
		std::filesystem::path filePath = std::filesystem::path(m_jsonRootDirectory).append(fileName); 
		json data = LoadJSONFile(filePath); 

		for (auto& [key, value] : data.items()) 
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data[key].contains("Type"), "JSON File: '{}' - Control with name '{}' has no 'Type' definition: {}", fileName, key, data[key].dump(4)); 
			JSON_LOADER_EXCEPTION_IF_FALSE(data[key]["Type"].is_string(), "JSON File: '{}' - Control with name '{}' has 'Type' definition that is not a string.\nInvalid value : {}", fileName, key, data[key]["Type"].dump(4)); 

			std::string type = data[key]["Type"].get<std::string>(); 

			if (IsLayoutKeyImpl(type))
			{
				// Clear all layout contents (rows, columns, controls, and sublayouts)
				layoutToFill->ClearContents();

				// Load the new contents
				//LoadLayoutDetails(layoutToFill->GetDeviceResources(), layoutToFill, data[key]);
				LoadLayoutImpl(layoutToFill->GetDeviceResources(), type, layoutToFill, data[key], key);
			}
			else
			{
				EG_CORE_WARN("JSON File: '{}' - Attempting to load layout - Type value must be 'Layout'. Invalid value: {}", fileName, type);
			}
		}

		// LayoutCheck is entirely optional - In a Release build, this does nothing
		layoutToFill->LayoutCheck();
	}
	catch (const JSONLoadersException& ex)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("Caught JSONLoadersException with message:\n{}", ex.what());
	}
	catch (const BaseException& ex)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("Caught BaseException with message:\n{}", ex.what());
	}
	catch (json::parse_error& e)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("Caught json::parse_error:\n{}", e.what());
	}
	catch (const std::exception& ex)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("Caught std::exception with message:\n{}", ex.what());
	}
	catch (...)
	{
		EG_CORE_ERROR("Failed to load control from file '{}'", fileName);
		EG_CORE_ERROR("{}", "Caught unidentified exception");
	}
}

json JSONLoaders::LoadJSONFile(std::filesystem::path filePath)
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

		// This can throw and somewhere up the call tree there needs to be a catch for json::parse_error
		return json::parse(fileData);
	}

	JSON_LOADER_EXCEPTION("Failed to open file '{}'", filePath.string());
	return {};
}
void JSONLoaders::LoadGlobalStyles(std::shared_ptr<DeviceResources> deviceResources)
{
	for (auto& [key, value] : m_jsonRoot.items())
	{
		if (m_jsonRoot[key].contains("Type"))
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(m_jsonRoot[key]["Type"].is_string(),	"Failed to parse global data for key '{}'. 'Type' field must be a string.\nInvalid value : {}", key, m_jsonRoot[key]["Type"].dump(4));

			std::string typeString = m_jsonRoot[key]["Type"].get<std::string>();

			if (JSONLoaders::IsStyleKey(typeString))
			{
				// Just calling LoadStyle is enough for the style to become cached within JSONLoaders.
				// Therefore, there is no reason to do anything with the return value.
				JSONLoaders::LoadStyle(deviceResources, typeString, m_jsonRoot[key], key);
			}
			else if (typeString.compare("Pane") == 0)
			{
				// Ignore Panes
				continue;
			}
			else if (typeString.size() > 5 && typeString.substr(typeString.size() - 5, 5).compare("Style") == 0) // If the typeString ends in "Style", warn the user they probably did not set up the StyleLoader correctly
			{
				EG_CORE_WARN("{}:{} - Found global style '{}' for key '{}', but did not file a StyleLoader within JSONLoaders. You may have forgotten to call JSONLoaders::AddStyleLoader()", __FILE__, __LINE__, typeString, key);
			}
		}
	}
}
void JSONLoaders::LoadPanes(std::shared_ptr<DeviceResources> deviceResources, Layout* layout)
{
	for (auto& [key, value] : m_jsonRoot.items())
	{
		if (m_jsonRoot[key].contains("Type"))
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(m_jsonRoot[key]["Type"].is_string(), "Failed to parse global data for key '{}'. 'Type' field must be a string.\nInvalid value : {}", key, m_jsonRoot[key]["Type"].dump(4));

			std::string typeString = m_jsonRoot[key]["Type"].get<std::string>();

			// Ignore everything that is not of Type 'Pane'
			if (typeString.compare("Pane") == 0)
			{
				ImportJSON(m_jsonRoot[key]);

				// Note: No need to create the control with LoadControl and then subsequently add the control to the UI's list of
				// Panes. PaneLoader will handle adding the new Pane to the UI list of Panes
				Control* pane = JSONLoaders::LoadControl(deviceResources, "Pane", layout, m_jsonRoot[key], key);
				if (pane == nullptr)
					EG_CORE_ERROR("Failed to load pane with name '{}'.", key);
			}
		}
	}
}
void JSONLoaders::ImportJSONImpl(json& data)
{
	if (data.contains("import"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["import"].is_string(), "'import' key must have a string-type value. Invalid value: {}", data["import"].dump(4));

		std::string importValue = data["import"].get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(importValue.size() > 0, "'import' key has an empty value. json data: {}", data.dump(4));

		json jsonImport;

		if (importValue.substr(importValue.size() - 5, 5).compare(".json") == 0)
		{
			jsonImport = LoadJSONFile(std::filesystem::path(m_jsonRootDirectory).append(importValue));
		}
		else
		{
			// 'import' value is a json key that should exist at the root level of the initial json object
			JSON_LOADER_EXCEPTION_IF_FALSE(m_jsonRoot.contains(importValue), "Unable to import key '{}'. Does not exist at the json root level.", importValue);

			jsonImport = m_jsonRoot[importValue];
		}

		// The imported json must be a json object
		JSON_LOADER_EXCEPTION_IF_FALSE(jsonImport.is_object(), "Unable to import key '{}'. Imported json value must be a json object.\nInvalid value: {}", importValue, jsonImport.dump(4));

		// Before adding the imported json object to our original json data, determine if it also has an 'import' key, and if so, recursively import the data
		ImportJSON(jsonImport);

		// Loop over the imported json object adding each field to the original object
		for (auto& [key, value] : jsonImport.items())
		{
			if (key.compare("import") == 0)
				continue;

			// Make sure there are no key conflicts
			JSON_LOADER_EXCEPTION_IF_FALSE(!data.contains(key), "Unable to import key '{}'. Both the original and imported json contain the same key: {}", importValue, key);

			data[key] = jsonImport[key];
		}

		// Finally, remove the 'import' key because so the parsing code can't accidentally attempt to import it a second time
		data.erase("import");
	}
}

void JSONLoaders::AddControlNameImpl(const std::string& name)
{
	for (const std::string& n : m_controlNames)
	{
		if (n.compare(name) == 0)
		{
			JSON_LOADER_EXCEPTION("Cannot duplicate control names. Offending name: {}", name);
		}
	}

	m_controlNames.push_back(name);
}

}