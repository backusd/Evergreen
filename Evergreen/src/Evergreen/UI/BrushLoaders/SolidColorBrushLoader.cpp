#include "pch.h"
#include "SolidColorBrushLoader.h"

/*
namespace Evergreen
{
SolidColorBrushLoader::SolidColorBrushLoader() noexcept
{
	m_keyLoaders["Color"] = [this](ColorBrush* colorBrush, json& data) -> bool { return this->ParseColor(static_cast<SolidColorBrush*>(colorBrush), data); };
}

std::optional<std::unique_ptr<ColorBrush>> SolidColorBrushLoader::Load(std::shared_ptr<DeviceResources> deviceResources, json& data) noexcept
{
	return Get().LoadImpl<SolidColorBrush>(deviceResources, data);
}

bool SolidColorBrushLoader::ParseColor(SolidColorBrush* brush, json& data) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, std::format("{}:{} - SolidColorBrush is not allowed to be nullptr", __FILE__, __LINE__));
	EG_CORE_ASSERT(data.contains("Color"), std::format("{}:{} - SolidColorBrush: json data must contain key 'Color'", __FILE__, __LINE__));

	// Color value can either be a string matching the D2D1::ColorF Enums, or a list of 3 or 4 floats for RGB[A] values
	if (data["Color"].is_string())
	{
		if (std::optional<D2D1_COLOR_F> colorOpt = Evergreen::ColorFromString(data["Color"].get<std::string>()))
		{
			brush->Color(colorOpt.value());
			return true;
		}

		EG_CORE_ERROR("{}:{} - SolidColorBrushLoader: Unrecognized Color: {}", __FILE__, __LINE__, data["Color"]);
		return false;
	}
	else if (data["Color"].is_array())
	{
		if (data["Color"].size() < 3 || data["Color"].size() > 4)
		{
			EG_CORE_ERROR("{}:{} - SolidColorBrushLoader: Color array must have exactly 3 or 4 float values: {}", __FILE__, __LINE__, data["Color"]);
			return false;
		}

		std::array<float, 4> rgba{ 1.0f, 1.0f, 1.0f, 1.0f };
		int iii = 0;
		for (auto& rgbaValue : data["Color"])
		{
			if (!rgbaValue.is_number_float())
			{
				EG_CORE_ERROR("{}:{} - SolidColorBrushLoader: Color array values must be floats. Invalid value: {}", __FILE__, __LINE__, rgbaValue);
				return false;
			}

			rgba[iii] = rgbaValue.get<float>();

			if (rgba[iii] < 0.0f || rgba[iii] > 1.0f)
			{
				EG_CORE_ERROR("{}:{} - SolidColorBrushLoader: Color array values must be between [0.0 - 1.0]. Invalid value: {}", __FILE__, __LINE__, rgba[iii]);
				return false;
			}

			++iii;
		}

		brush->Color(D2D1::ColorF(rgba[0], rgba[1], rgba[2], rgba[3]));
		return true;
	}	

	EG_CORE_ERROR("{}:{} - SolidColorBrushLoader: Invalid Color Value: {} - Must either be a string or array of RGB[A] values between 0.0 - 1.0", __FILE__, __LINE__, data["Color"]);
	return false;
}

}
*/