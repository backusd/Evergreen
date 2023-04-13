#include "pch.h"
#include "SolidColorBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
SolidColorBrush::SolidColorBrush(std::shared_ptr<DeviceResources> deviceResources, const D2D1_COLOR_F& solidColor) :
	ColorBrush(deviceResources), 
	m_color(solidColor)
{
	Refresh();
}
SolidColorBrush::SolidColorBrush(const SolidColorBrush& rhs) :
	ColorBrush(rhs.m_deviceResources),
	m_color(rhs.m_color)
{
	Refresh();
}
SolidColorBrush& SolidColorBrush::operator=(const SolidColorBrush & rhs)
{
	ColorBrush::operator=(rhs);

	m_color = rhs.m_color;
	Refresh();

	return *this;
}

std::unique_ptr<ColorBrush> SolidColorBrush::Duplicate()
{
	return std::move(std::make_unique<SolidColorBrush>(*this));
}

void SolidColorBrush::Refresh()
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	ComPtr<ID2D1SolidColorBrush> solidBrush;
	GFX_THROW_INFO(
		m_deviceResources->D2DDeviceContext()->CreateSolidColorBrush(
			m_color,
			m_brushProperties,
			solidBrush.ReleaseAndGetAddressOf()
		)
	)

	GFX_THROW_INFO(
		solidBrush->QueryInterface<ID2D1Brush>(m_brush.ReleaseAndGetAddressOf())
	)
}

void SolidColorBrush::Color(float r, float g, float b, float a)
{
	m_color = D2D1::ColorF(r, g, b, a);
	Refresh();
}
void SolidColorBrush::Color(D2D1::ColorF::Enum colorEnum)
{
	m_color = D2D1::ColorF(colorEnum);
	Refresh();
}
void SolidColorBrush::Color(const D2D1_COLOR_F& color)
{
	m_color = color;
	Refresh();
}



}