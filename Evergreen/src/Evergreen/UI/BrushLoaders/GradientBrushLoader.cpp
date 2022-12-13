#include "pch.h"
#include "GradientBrushLoader.h"

/*
namespace Evergreen
{
GradientBrushLoader::GradientBrushLoader() noexcept
{
	m_keyLoaders["Stops"] = [this](ColorBrush* colorBrush, json& data) -> bool { return this->ParseStops(static_cast<GradientBrush*>(colorBrush), data); };
	m_keyLoaders["Gamma"] = [this](ColorBrush* colorBrush, json& data) -> bool { return this->ParseGamma(static_cast<GradientBrush*>(colorBrush), data); };
	m_keyLoaders["ExtendMode"] = [this](ColorBrush* colorBrush, json& data) -> bool { return this->ParseExtendMode(static_cast<GradientBrush*>(colorBrush), data); };
	m_keyLoaders["Axis"] = [this](ColorBrush* colorBrush, json& data) -> bool { return this->ParseAxis(static_cast<GradientBrush*>(colorBrush), data); };
}

std::optional<std::unique_ptr<ColorBrush>> GradientBrushLoader::Load(std::shared_ptr<DeviceResources> deviceResources, json& data) noexcept
{
	return Get().LoadImpl<GradientBrush>(deviceResources, data);
}

bool GradientBrushLoader::ParseStops(GradientBrush* brush, json& data) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, std::format("{}:{} - GradientBrush is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("Stops"), std::format("{}:{} - GradientBrush: json data must contain key 'Stops'", __FILE__, __LINE__));

	if (!data["Stops"].is_array())
	{
		EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'Stops' value must be an array. Invalid value: {}", __FILE__, __LINE__, data["Stops"]);
		return false;
	}

	for (auto& stopData : data["Stops"])
	{
		D2D1_GRADIENT_STOP stop;

		if (!stopData.is_object())
		{
			EG_CORE_ERROR("{}:{} - GradientBrushLoader: Each 'Stops' item must be json object. Invalid value: {}", __FILE__, __LINE__, stopData);
			return false;
		}

		if (!stopData.contains("Color"))
		{
			EG_CORE_ERROR("{}:{} - GradientBrushLoader: Each 'Stops' item must contain a 'Color' key. Incomplete stop object: {}", __FILE__, __LINE__, stopData);
			return false;
		}

		if (!stopData.contains("Position"))
		{
			EG_CORE_ERROR("{}:{} - GradientBrushLoader: Each 'Stops' item must contain a 'Position' key. Incomplete stop object: {}", __FILE__, __LINE__, stopData);
			return false;
		}

		for (auto& [key, value] : stopData.items())
		{
			if (key.compare("Color") == 0 || key.compare("Position") == 0)
				continue;

			EG_CORE_WARN("{}:{} - GradientBrushLoader: Skipping invalid 'Stops' object key: '{}'", __FILE__, __LINE__, key);
		}

		if (std::optional<D2D1_COLOR_F> colorOpt = ParseColor(stopData["Color"]))
		{
			stop.color = colorOpt.value();
		}
		else
		{
			EG_CORE_ERROR("{}:{} - GradientBrushLoader: ParseColor() returned nullopt for 'Color' value: {}", __FILE__, __LINE__, stopData["Color"]);
			return false;
		}

		if (!stopData["Position"].is_number_float())
		{
			EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'Stops' item 'Position' key must be a float. Invalid value: {}", __FILE__, __LINE__, stopData["Position"]);
			return false;
		}

		stop.position = stopData["Position"].get<float>();

		if (stop.position < 0.0f || stop.position > 1.0f)
		{
			EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'Stops' item 'Position' key must be in the range [0.0 - 1.0]. Invalid value: {}", __FILE__, __LINE__, stop.position);
			return false;
		}

		brush->AddStop(stop);
	}

	return true;
}
bool GradientBrushLoader::ParseGamma(GradientBrush* brush, json& data) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, std::format("{}:{} - GradientBrush is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("Gamma"), std::format("{}:{} - GradientBrush: json data must contain key 'Gamma'", __FILE__, __LINE__));

	if (!data["Gamma"].is_string())
	{
		EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'Gamma' value must be an string. Invalid value: {}", __FILE__, __LINE__, data["Gamma"]);
		return false;
	}

	std::string gamma = data["Gamma"].get<std::string>();

	if (gamma.compare("D2D1_GAMMA_2_2") == 0)
	{
		brush->Gamma(D2D1_GAMMA_2_2);
		return true;
	}

	if (gamma.compare("D2D1_GAMMA_1_0") == 0)
	{
		brush->Gamma(D2D1_GAMMA_1_0);
		return true;
	}

	EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'Gamma' value must be either D2D1_GAMMA_1_0 or D2D1_GAMMA_2_2. Invalid value: {}", __FILE__, __LINE__, gamma);
	return false;
}
bool GradientBrushLoader::ParseExtendMode(GradientBrush* brush, json& data) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, std::format("{}:{} - GradientBrush is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("ExtendMode"), std::format("{}:{} - GradientBrush: json data must contain key 'ExtendMode'", __FILE__, __LINE__));

	if (!data["ExtendMode"].is_string())
	{
		EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'ExtendMode' value must be an string. Invalid value: {}", __FILE__, __LINE__, data["ExtendMode"]);
		return false;
	}

	std::string extendMode = data["ExtendMode"].get<std::string>();

	if (extendMode.compare("D2D1_EXTEND_MODE_CLAMP") == 0)
	{
		brush->ExtendMode(D2D1_EXTEND_MODE_CLAMP);
		return true;
	}
	if (extendMode.compare("D2D1_EXTEND_MODE_WRAP") == 0)
	{
		brush->ExtendMode(D2D1_EXTEND_MODE_WRAP);
		return true;
	}
	if (extendMode.compare("D2D1_EXTEND_MODE_MIRROR") == 0)
	{
		brush->ExtendMode(D2D1_EXTEND_MODE_MIRROR);
		return true;
	}

	EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'ExtendMode' value must be one of D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_WRAP, or D2D1_EXTEND_MODE_MIRROR. Invalid value: {}", __FILE__, __LINE__, extendMode);
	return false;
}
bool GradientBrushLoader::ParseAxis(GradientBrush* brush, json& data) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, std::format("{}:{} - GradientBrush is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("Axis"), std::format("{}:{} - GradientBrush: json data must contain key 'Axis'", __FILE__, __LINE__));

	if (!data["Axis"].is_string())
	{
		EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'Axis' value must be an string. Invalid value: {}", __FILE__, __LINE__, data["Axis"]);
		return false;
	}

	std::string axis = data["Axis"].get<std::string>();

	if (axis.compare("Horizontal") == 0)
	{
		brush->GradientAxis(GRADIENT_AXIS::HORIZONTAL);
		return true;
	}
	if (axis.compare("Vertical") == 0)
	{
		brush->GradientAxis(GRADIENT_AXIS::VERTICAL);
		return true;
	}
	if (axis.compare("NESW") == 0)
	{
		brush->GradientAxis(GRADIENT_AXIS::NESW);
		return true;
	}
	if (axis.compare("NWSE") == 0)
	{
		brush->GradientAxis(GRADIENT_AXIS::NWSE);
		return true;
	}

	EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'Axis' value must be one of 'Horizontal', 'Vertical', 'NESW', 'NWSE'. Invalid value: {}", __FILE__, __LINE__, axis);
	return false;
}


std::optional<D2D1_COLOR_F> GradientBrushLoader::ParseColor(json& data) noexcept
{
	// Color value can either be a string matching the D2D1::ColorF Enums, or a list of 3 or 4 floats for RGB[A] values
	if (data.is_string())
	{
		if (std::optional<D2D1_COLOR_F> colorOpt = Evergreen::ColorFromString(data.get<std::string>()))
			return colorOpt.value();

		EG_CORE_ERROR("{}:{} - GradientBrushLoader: Unrecognized Color: {}", __FILE__, __LINE__, data);
		return std::nullopt;
	}
	else if (data.is_array())
	{
		if (data.size() < 3 || data.size() > 4)
		{
			EG_CORE_ERROR("{}:{} - GradientBrushLoader: Color array must have exactly 3 or 4 float values. Invalid data: {}", __FILE__, __LINE__, data);
			return std::nullopt;
		}

		std::array<float, 4> rgba{ 1.0f, 1.0f, 1.0f, 1.0f };
		int iii = 0;
		for (auto& rgbaValue : data)
		{
			if (!rgbaValue.is_number_float())
			{
				EG_CORE_ERROR("{}:{} - GradientBrushLoader: Color array values must be floats. Invalid value: {}", __FILE__, __LINE__, rgbaValue);
				return std::nullopt;
			}

			rgba[iii] = rgbaValue.get<float>();

			if (rgba[iii] < 0.0f || rgba[iii] > 1.0f)
			{
				EG_CORE_ERROR("{}:{} - GradientBrushLoader: Color array values must be between [0.0 - 1.0]. Invalid value: {}", __FILE__, __LINE__, rgba[iii]);
				return std::nullopt;
			}

			++iii;
		}

		return D2D1::ColorF(rgba[0], rgba[1], rgba[2], rgba[3]);
	}


	EG_CORE_ERROR("{}:{} - GradientBrushLoader: 'Color' value must be either string type or array of floats. Invalid value: {}", __FILE__, __LINE__, data);
	return std::nullopt;
}

}
*/