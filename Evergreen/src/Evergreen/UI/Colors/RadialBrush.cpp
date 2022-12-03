#include "pch.h"
#include "RadialBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
// This constructor is for a gradient with exactly 2 colors - 1 at the center and 1 at the edge
RadialBrush::RadialBrush(std::shared_ptr<DeviceResources> deviceResources, const D2D1_COLOR_F& startColor, const D2D1_COLOR_F& endColor,
	const D2D1_POINT_2F& center, float radiusX, float radiusY, D2D1_EXTEND_MODE extendMode, D2D1_GAMMA gamma) noexcept :
	ColorBrush(deviceResources),
	m_stops{ {0.0f, startColor}, {1.0f, endColor} },
	m_center(center),
	m_gradientOriginOffset{0.0f, 0.0f},
	m_radiusX(radiusX),
	m_radiusY(radiusY),
	m_extendMode(extendMode),
	m_gamma(gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	Refresh();
}
RadialBrush::RadialBrush(std::shared_ptr<DeviceResources> deviceResources, const std::vector<D2D1_GRADIENT_STOP>& stops, const D2D1_POINT_2F& center, 
	float radiusX, float radiusY, const D2D1_POINT_2F& gradientOriginOffset, D2D1_EXTEND_MODE extendMode, D2D1_GAMMA gamma) noexcept :
	ColorBrush(deviceResources),
	m_stops(stops),
	m_center(center),
	m_gradientOriginOffset(gradientOriginOffset),
	m_radiusX(radiusX),
	m_radiusY(radiusY),
	m_extendMode(extendMode),
	m_gamma(gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	Refresh();
}
RadialBrush::RadialBrush(const RadialBrush& rhs) noexcept :
	ColorBrush(rhs.m_deviceResources),
	m_stops(rhs.m_stops),
	m_center(rhs.m_center),
	m_gradientOriginOffset(rhs.m_gradientOriginOffset),
	m_radiusX(rhs.m_radiusX),
	m_radiusY(rhs.m_radiusY),
	m_extendMode(rhs.m_extendMode),
	m_gamma(rhs.m_gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	Refresh();
}
void RadialBrush::operator=(const RadialBrush& rhs) noexcept
{
	m_deviceResources = rhs.m_deviceResources;
	m_brush = nullptr;
	m_stops = rhs.m_stops;
	m_center = rhs.m_center;
	m_gradientOriginOffset = rhs.m_gradientOriginOffset;
	m_radiusX = rhs.m_radiusX;
	m_radiusY = rhs.m_radiusY;
	m_extendMode = rhs.m_extendMode;
	m_gamma = rhs.m_gamma;
	m_pGradientStops = nullptr;

	RefreshGradientStops();
	Refresh();
}

void RadialBrush::RefreshGradientStops() noexcept
{
	// Refresh() will be called when a layout/control changes size and will need to update
	// the gradient start/end. To avoid the unnecessary cost of recreating the gradient stops,
	// that functionality has been moved to this function.
	GFX_THROW_INFO(
		m_deviceResources->D2DDeviceContext()->CreateGradientStopCollection(
			m_stops.data(),
			static_cast<UINT32>(m_stops.size()),
			m_gamma,
			m_extendMode,
			m_pGradientStops.ReleaseAndGetAddressOf()
		)
	)
}
void RadialBrush::Refresh() noexcept
{
	// Refresh may need to be called when the mouse moves and will need to update
	// the brush. To avoid the unnecessary cost of recreating the gradient stops,
	// that functionality has been moved out of this function.

	ComPtr<ID2D1RadialGradientBrush> radialBrush = nullptr;
	GFX_THROW_INFO(
		m_deviceResources->D2DDeviceContext()->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(
				m_center,
				m_gradientOriginOffset,
				m_radiusX,
				m_radiusY),
			m_brushProperties,
			m_pGradientStops.Get(),
			radialBrush.ReleaseAndGetAddressOf()
		)
	)

	GFX_THROW_INFO(
		radialBrush->QueryInterface<ID2D1Brush>(m_brush.ReleaseAndGetAddressOf())
	)
}


}