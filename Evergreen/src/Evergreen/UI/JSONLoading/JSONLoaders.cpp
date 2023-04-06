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
		D2D1_GRADIENT_STOP stop;

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
		D2D1_GRADIENT_STOP stop;

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

bool JSONLoaders::LoadUIImpl(std::shared_ptr<DeviceResources> deviceResources, const std::filesystem::path& rootDirectory, const std::string& rootFile, Layout* rootLayout) noexcept
{
	try
	{
		// Store root directory as member variable so ImportJson can use it
		m_jsonRootDirectory = rootDirectory;
		std::filesystem::path rootFilePath = std::filesystem::path(rootDirectory).append(rootFile);

		m_jsonRoot = LoadJSONFile(rootFilePath);

		// Before constructing the layout, load global data that can be retrieved later on
		LoadGlobalStyles(deviceResources);

		// Load all the json data under the 'root' key
 		LoadLayoutDetails(deviceResources, rootLayout, m_jsonRoot["root"]);

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
void JSONLoaders::LoadLayoutDetails(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data)
{
	// First, import any necessary data
	ImportJSON(data);

	// Load layout brush
	LoadLayoutBrush(deviceResources, layout, data);

	// Load Row/Column definitions
	LoadLayoutRowDefinitions(layout, data);
	LoadLayoutColumnDefinitions(layout, data);

	// Load Margin
	LoadLayoutMargin(layout, data);

	// Load Layout Border details
	LoadLayoutBorder(deviceResources, layout, data);

	// Load Layout Callbacks
	LoadLayoutCallbacks(layout, data);

	// Now iterate over the controls and sublayouts within the layout
	for (auto& [key, value] : data.items())
	{
		if (key.compare("import") == 0 ||
			key.compare("id") == 0 ||
			key.compare("Type") == 0 ||
			key.compare("Brush") == 0 ||
			key.compare("Row") == 0 ||
			key.compare("Column") == 0 ||
			key.compare("RowSpan") == 0 ||
			key.compare("ColumnSpan") == 0 ||
			key.compare("RowDefinitions") == 0 ||
			key.compare("ColumnDefinitions") == 0 ||
			key.compare("Margin") == 0 ||
			key.compare("BorderBrush") == 0 ||
			key.compare("BorderWidth") == 0 ||
			key.compare("BorderTopLeftOffsetX") == 0 || 
			key.compare("BorderTopLeftOffsetY") == 0 ||
			key.compare("BorderTopRightOffsetX") == 0 ||
			key.compare("BorderTopRightOffsetY") == 0 ||
			key.compare("BorderBottomLeftOffsetX") == 0 ||
			key.compare("BorderBottomLeftOffsetY") == 0 ||
			key.compare("BorderBottomRightOffsetX") == 0 ||
			key.compare("BorderBottomRightOffsetY") == 0 ||
			key.compare("OnResize") == 0)
			continue;

		JSON_LOADER_EXCEPTION_IF_FALSE(data[key].contains("Type"), "Control or sub-layout has no 'Type' definition: {}", data[key].dump(4));
		JSON_LOADER_EXCEPTION_IF_FALSE(data[key]["Type"].is_string(), "Control or sub-layout 'Type' definition must be a string.\nInvalid value : {}", data[key]["Type"].dump(4));

		std::string type = data[key]["Type"].get<std::string>();

		// Load either a sublayout or control
		if (type.compare("Layout") == 0)
		{
			LoadSubLayout(deviceResources, layout, data[key], key);
		}
		else if (JSONLoaders::IsControlKey(type))
		{
			ImportJSON(data[key]);

			Control* control = JSONLoaders::LoadControl(deviceResources, type, layout, data[key], key);
			if (control == nullptr)
				EG_CORE_ERROR("Failed to load control with name '{}'.", key);
		}
		else
		{
			EG_CORE_WARN("Attempting to load control: {} (... not yet supported ...)", type);
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
void JSONLoaders::LoadLayoutBrush(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "Layout cannot be nullptr");

	if (data.contains("Brush"))
	{
		std::unique_ptr<ColorBrush> brush = JSONLoaders::LoadBrush(deviceResources, data["Brush"]);
		layout->BackgroundBrush(std::move(brush));
	}
}
void JSONLoaders::LoadLayoutRowDefinitions(Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "Layout cannot be nullptr");

	// The 'RowDefinitions' parameter is not required
	// If not included, a single row will be created that spans the layout
	if (data.contains("RowDefinitions"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RowDefinitions"].is_array(), "RowDefinitions value must be an array type. Invalid value: {}", data["RowDefinitions"].dump(4));

		// Iterate the list of row definitions
		for (auto& rowDefinition : data["RowDefinitions"])
		{
			// Each row must contain a 'Height' key
			JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition.contains("Height"), "RowDefinition for layout '{}' does not contain 'Height' key: {}", layout->Name(), rowDefinition.dump(4));

			// Get the Row type and size
			auto [rowColType, rowColSize] = ParseRowColumnTypeAndSize(rowDefinition["Height"], layout->Name());

			// Create the row and get a pointer to it so we can edit it further
			Row* row = layout->AddRow({ rowColType, rowColSize });

			// Before looping over all the row definition key/value pairs, first check if we need to import other json data
			ImportJSON(rowDefinition);

			// Iterate over the other keys in the row definition
			for (auto& [key, value] : rowDefinition.items())
			{
				if (key.compare("Height") == 0 || key.compare("comment") == 0 || key.compare("import") == 0)
					continue;
				else if (key.compare("TopAdjustable") == 0)
				{
					JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition[key].is_boolean(), "RowDefinition TopAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), rowDefinition[key].dump(4));
					row->TopIsAdjustable(rowDefinition[key].get<bool>());
				}
				else if (key.compare("BottomAdjustable") == 0)
				{
					JSON_LOADER_EXCEPTION_IF_FALSE(rowDefinition[key].is_boolean(), "RowDefinition BottomAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), rowDefinition[key].dump(4));
					row->BottomIsAdjustable(rowDefinition[key].get<bool>());
				}
				else if (key.compare("MaxHeight") == 0)
				{
					auto [type, size] = ParseRowColumnTypeAndSize(rowDefinition[key], layout->Name());
					row->MaxHeight({ type, size });
				}
				else if (key.compare("MinHeight") == 0)
				{
					auto [type, size] = ParseRowColumnTypeAndSize(rowDefinition[key], layout->Name());
					row->MinHeight({ type, size });
				}
				else
				{
					// Just a warning because the key will be ignored
					EG_CORE_WARN("{}:{} - Unrecognized key ({}) in RowDefinition for layout '{}'.", __FILE__, __LINE__, key, layout->Name());
				}
			}

		}
	}
	else
	{
		// Add a single row that spans the layout
		layout->AddRow({ RowColumnType::STAR, 1.0f });
	}
}
void JSONLoaders::LoadLayoutColumnDefinitions(Layout* layout, json& data)
{
	// The 'ColumnDefinitions' parameter is not required
	// If not included, a single column will be created that spans the layout
	if (data.contains("ColumnDefinitions"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ColumnDefinitions"].is_array(), "ColumnDefinitions value must be an array type. Invalid value: {}", data["ColumnDefinitions"].dump(4));

		// Iterate the list of column definitions
		for (auto& columnDefinition : data["ColumnDefinitions"])
		{
			// Each column must contain a 'Width' key
			JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition.contains("Width"), "ColumnDefinition for layout '{}' does not contain 'Width' key: {}", layout->Name(), columnDefinition.dump(4));

			// Get the Row type and size
			auto [rowColType, rowColSize] = ParseRowColumnTypeAndSize(columnDefinition["Width"], layout->Name());

			// Create the column and get a pointer to it so we can edit it further
			Column* column = layout->AddColumn({ rowColType, rowColSize });

			// Before looping over all the row definition key/value pairs, first check if we need to import other json data
			ImportJSON(columnDefinition);

			// Iterate over the other keys in the column definition
			for (auto& [key, value] : columnDefinition.items())
			{
				if (key.compare("Width") == 0 || key.compare("comment") == 0 || key.compare("import") == 0)
					continue;
				else if (key.compare("LeftAdjustable") == 0)
				{
					JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition[key].is_boolean(), "ColumnDefinition LeftAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), columnDefinition[key].dump(4));
					column->LeftIsAdjustable(columnDefinition[key].get<bool>());
				}
				else if (key.compare("RightAdjustable") == 0)
				{
					JSON_LOADER_EXCEPTION_IF_FALSE(columnDefinition[key].is_boolean(), "ColumnDefinition RightAdjustable field for layout '{}' must have boolean type.\nInvalid value: {}", layout->Name(), columnDefinition[key].dump(4));
					column->RightIsAdjustable(columnDefinition[key].get<bool>());
				}
				else if (key.compare("MaxWidth") == 0)
				{
					auto [type, size] = ParseRowColumnTypeAndSize(columnDefinition[key], layout->Name());
					column->MaxWidth({ type, size });
				}
				else if (key.compare("MinWidth") == 0)
				{
					auto [type, size] = ParseRowColumnTypeAndSize(columnDefinition[key], layout->Name());
					column->MinWidth({ type, size });
				}
				else
				{
					// Just a warning because the key will be ignored
					EG_CORE_WARN("{}:{} - Unrecognized key ({}) in ColumnDefinition for layout '{}'.", __FILE__, __LINE__, key, layout->Name());
				}
			}
		}
	}
	else
	{
		// Add a single column that spans the layout
		layout->AddColumn({ RowColumnType::STAR, 1.0f });
	}
}
void JSONLoaders::LoadLayoutBorder(std::shared_ptr<DeviceResources> deviceResources, Layout* layout, json& data)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(layout != nullptr, "No layout");

	// Border Brush -----------------------------------------------------------------------------------
	if (data.contains("BorderWidth")) 
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data.contains("BorderBrush"), "Layout with name '{}': When 'BorderWidth' key is present, the 'BorderBrush' key is required. Incomplete Layout object: {}", layout->Name(), data.dump(4));
	}
	if (data.contains("BorderBrush"))
	{
		layout->BorderBrush(std::move(JSONLoaders::LoadBrush(deviceResources, data["BorderBrush"]))); 
	}

	// Border Width -----------------------------------------------------------------------------------
	std::array<float, 4> borderWidths{ 0.0f, 0.0f, 0.0f, 0.0f }; 
	if (data.contains("BorderWidth")) 
	{
		if (data["BorderWidth"].is_number()) 
		{
			float width = data["BorderWidth"].get<float>(); 
			JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "Layout with name '{}': 'BorderWidth' is not allowed to be less than 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
			borderWidths.fill(width);
			layout->BorderWidth(borderWidths);
		}
		else if (data["BorderWidth"].is_array())
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"].size() == 4, "Layout with name '{}': When specificying 'BorderWidth' as an array of floats, there must be exactly 4 values in the array. Invalid Layout object: {}", layout->Name(), data.dump(4));

			float width = 0.0f; 
			for (unsigned int iii = 0; iii < 4; ++iii) 
			{
				JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderWidth"][iii].is_number(), "Layout with name '{}': When specificying 'BorderWidth' as an array, the array values must be parsable numbers. Invalid Layout object: {}", layout->Name(), data.dump(4));
				width = data["BorderWidth"][iii].get<float>(); 
				JSON_LOADER_EXCEPTION_IF_FALSE(width >= 0.0f, "Layout with name '{}': 'BorderWidth' array values are not allowed to be less than 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
				borderWidths[iii] = width; 
			}
			layout->BorderWidth(borderWidths); 
		}
		else
		{
			JSON_LOADER_EXCEPTION("Layout with name '{}': 'BorderWidth' value must either be a number of an array of 4 floats. Invalid Layout object: {}", layout->Name(), data.dump(4));
		}
	}

	// Border Offsets -----------------------------------------------------------------------------------
	if (data.contains("BorderTopLeftOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopLeftOffsetX"].is_number(), "Layout with name '{}': 'BorderTopLeftOffsetX' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderTopLeftOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderTopLeftOffsetX' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderTopLeftOffsetX(value);
	}
	if (data.contains("BorderTopLeftOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopLeftOffsetY"].is_number(), "Layout with name '{}': 'BorderTopLeftOffsetY' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderTopLeftOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderTopLeftOffsetY' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderTopLeftOffsetY(value);
	}
	if (data.contains("BorderTopRightOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopRightOffsetX"].is_number(), "Layout with name '{}': 'BorderTopRightOffsetX' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderTopRightOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderTopRightOffsetX' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderTopRightOffsetX(value);
	}
	if (data.contains("BorderTopRightOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderTopRightOffsetY"].is_number(), "Layout with name '{}': 'BorderTopRightOffsetY' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderTopRightOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderTopRightOffsetY' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderTopRightOffsetY(value);
	}
	if (data.contains("BorderBottomLeftOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomLeftOffsetX"].is_number(), "Layout with name '{}': 'BorderBottomLeftOffsetX' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderBottomLeftOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderBottomLeftOffsetX' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderBottomLeftOffsetX(value);
	}
	if (data.contains("BorderBottomLeftOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomLeftOffsetY"].is_number(), "Layout with name '{}': 'BorderBottomLeftOffsetY' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderBottomLeftOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderBottomLeftOffsetY' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderBottomLeftOffsetY(value);
	}
	if (data.contains("BorderBottomRightOffsetX"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomRightOffsetX"].is_number(), "Layout with name '{}': 'BorderBottomRightOffsetX' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderBottomRightOffsetX"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderBottomRightOffsetX' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderBottomRightOffsetX(value);
	}
	if (data.contains("BorderBottomRightOffsetY"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["BorderBottomRightOffsetY"].is_number(), "Layout with name '{}': 'BorderBottomRightOffsetY' value must be a number. Invalid Layout object: {}", layout->Name(), data.dump(4));
		float value = data["BorderBottomRightOffsetY"].get<float>();
		JSON_LOADER_EXCEPTION_IF_FALSE(value >= 0.0f, "Layout with name '{}': 'BorderBottomRightOffsetY' value must be >= 0. Invalid Layout object: {}", layout->Name(), data.dump(4));
		layout->BorderBottomRightOffsetY(value);
	}
}
void JSONLoaders::LoadLayoutCallbacks(Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "No layout");

	if (data.contains("OnResize"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["OnResize"].is_string(), "Layout with name '{}': 'OnResize' value must be a string. Invalid Layout object: {}", layout->Name(), data.dump(4));

		std::string key = data["OnResize"].get<std::string>();

		auto callback = JSONLoaders::GetLayoutCallback(key);
		JSON_LOADER_EXCEPTION_IF_FALSE(callback != nullptr, "Layout with name '{}': 'OnResize' callback not found for key '{}'. Invalid Layout object: {}", layout->Name(), key, data.dump(4));
		layout->SetOnResizeCallback(callback);

		// NOTE: It might be tempting to immediately call layout->TriggerOnResizeCallback() to allow the
		//		 callback to alter the layout in some way. However, it is very possible that the callback
		//		 will perform a lookup of another Layout or Control that has not yet been created. Therefore,
		//		 we MUST defer triggering the callback until the entire UI has been loaded.
	}
}
void JSONLoaders::LoadLayoutID(Layout* layout, json& data)
{
	EG_CORE_ASSERT(layout != nullptr, "No layout");
	
	if (data.contains("id"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["id"].is_number_unsigned(), "Layout with name: '{}'. 'id' value must be an unsigned int. Invalid data: {}", layout->Name(), data.dump(4));
		layout->ID(data["id"].get<unsigned int>());
	}
}
void JSONLoaders::LoadLayoutMargin(Layout* layout, json& data)
{
	if (data.contains("Margin"))
	{
		Margin margin{ 0 };
		json& marginData = data["Margin"];

		JSON_LOADER_EXCEPTION_IF_FALSE(marginData.is_array(), "Layout with name: '{}'. 'Margin' value must be an array. Invalid data: {}", layout->Name(), data.dump(4));

		for (auto& marginValue : marginData)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(marginValue.is_number(), "Layout with name: '{}'. 'Margin' array values must be numbers. Invalid data: {}", layout->Name(), data.dump(4));
		}

		if (marginData.size() == 1)
		{
			margin.Bottom = marginData[0].get<float>();
			margin.Left = margin.Bottom;
			margin.Right = margin.Bottom;
			margin.Top = margin.Bottom;
		}
		else if (marginData.size() == 2)
		{
			margin.Left = marginData[0].get<float>();
			margin.Right = margin.Left;

			margin.Top = marginData[1].get<float>();
			margin.Bottom = margin.Top;
		}
		else if (marginData.size() == 4)
		{
			margin.Left = marginData[0].get<float>();
			margin.Top = marginData[1].get<float>();
			margin.Right = marginData[2].get<float>();
			margin.Bottom = marginData[3].get<float>();
		}
		else
		{
			JSON_LOADER_EXCEPTION("Layout with name: '{}'. 'Margin' array must have 1, 2, or 4 values. Invalid data: {}", layout->Name(), data.dump(4));
		}

		layout->Margin(margin);
	}
}
void JSONLoaders::LoadSubLayout(std::shared_ptr<DeviceResources> deviceResources, Layout* parent, json& data, const std::string& name)
{
	// First, import any necessary data
	ImportJSON(data);

	// Parse the row/column/rowspan/columnspan values
	RowColumnPosition position = ParseRowColumnPosition(data);

	Layout* sublayout = parent->AddSubLayout(position, name);
	return LoadLayoutDetails(deviceResources, sublayout, data);
}

RowColumnPosition JSONLoaders::ParseRowColumnPosition(json& data)
{
	RowColumnPosition position;
	position.Row = 0;
	position.Column = 0;
	position.RowSpan = 1;
	position.ColumnSpan = 1;

	if (data.contains("Row"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Row"].is_number_unsigned(), "'Row' value must be an unsigned int.\nInvalid value: {}", data["Row"].dump(4));
		position.Row = data["Row"].get<unsigned int>();
	}

	if (data.contains("Column"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Column"].is_number_unsigned(), "'Column' value must be an unsigned int.\nInvalid value: {}", data["Column"].dump(4));
		position.Column = data["Column"].get<unsigned int>();
	}

	if (data.contains("RowSpan"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RowSpan"].is_number_unsigned(), "'RowSpan' value must be an unsigned int.\nInvalid value : {}", data["RowSpan"].dump(4));

		position.RowSpan = data["RowSpan"].get<unsigned int>();

		if (position.RowSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'RowSpan' with value of 0. Setting rowSpan = 1", __FILE__, __LINE__);
			position.RowSpan = 1;
		}
	}

	if (data.contains("ColumnSpan"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ColumnSpan"].is_number_unsigned(), "'ColumnSpan' value must be an unsigned int.\nInvalid value : {}", data["ColumnSpan"].dump(4));

		position.ColumnSpan = data["ColumnSpan"].get<unsigned int>();

		if (position.ColumnSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'ColumnSpan' with value of 0. Setting columnSpan = 1", __FILE__, __LINE__);
			position.ColumnSpan = 1;
		}
	}

	return position;
}
std::tuple<RowColumnType, float> JSONLoaders::ParseRowColumnTypeAndSizeImpl(json& data, const std::string& layoutName)
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

		JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Fixed Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layoutName, size);

		return std::make_tuple(type, size);
	}
	else if (data.is_string())
	{
		std::string dataString = data.get<std::string>();

		JSON_LOADER_EXCEPTION_IF_FALSE(dataString.size() > 0, "RowDefinition Height/Width value for layout '{}' must not be empty.", layoutName);

		try {
			// Don't need to take a substring of datastring to remove '%' or '*' because stof will read all valid
			// digits and stop once it hits the end of the string or a non-numeric character
			size = std::stof(dataString);
		}
		catch (const std::invalid_argument& e)
		{
			JSON_LOADER_EXCEPTION("Invalid Height/Width value for layout ({}) could not be parsed to a float: {}.\nException Message: {}", layoutName, dataString, e.what());
		}
		catch (const std::out_of_range& e)
		{
			JSON_LOADER_EXCEPTION("Invalid Height/Width value for layout ({}). Caught out of range exception for value '{}'.\nException Message: {}", layoutName, dataString, e.what());
		}

		// Get the type and perform some bounds checking
		switch (dataString.back())
		{
		case '%':
			type = RowColumnType::PERCENT;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f && size <= 100.0f, "Percent Height/Width value for layout '{}' must be in the range (0, 100].\nInvalid value: {}", layoutName, size);

			size /= 100.0f; // Must divide by 100 to stay within the range [0, 1]

			break;
		case '*':
			type = RowColumnType::STAR;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Star Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layoutName, size);

			break;
		default:
			type = RowColumnType::FIXED;

			JSON_LOADER_EXCEPTION_IF_FALSE(size > 0.0f, "Fixed Height/Width value for layout '{}' must be greater than 0.\nInvalid value: {}", layoutName, size);

			break;
		}

		return std::make_tuple(type, size);
	}

	JSON_LOADER_EXCEPTION("Height/Width value must be either a number or a string.\nLayout Name: {}\nInvalid value: {}", layoutName, data.dump(4));
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