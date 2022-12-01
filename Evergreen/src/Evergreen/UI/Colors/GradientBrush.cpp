#include "pch.h"
#include "GradientBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
// This constructor is for a gradient with exactly 2 colors - 1 at the beginning and 1 at the end
GradientBrush::GradientBrush(std::shared_ptr<DeviceResources> deviceResources, const D2D1_COLOR_F& startColor, const D2D1_COLOR_F& endColor, 
	const D2D1_POINT_2F& gradientAxisStart, const D2D1_POINT_2F& gradientAxisEnd, D2D1_EXTEND_MODE extendMode, D2D1_GAMMA gamma) noexcept :
	ColorBrush(deviceResources),
	m_stops{{0.0f, startColor}, {1.0f, endColor}},
	m_gradientAxisStart(gradientAxisStart),
	m_gradientAxisEnd(gradientAxisEnd),
	m_extendMode(extendMode),
	m_gamma(gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	Refresh();
}
GradientBrush::GradientBrush(std::shared_ptr<DeviceResources> deviceResources, const std::vector<D2D1_GRADIENT_STOP>& stops,
	const D2D1_POINT_2F& gradientAxisStart, const D2D1_POINT_2F& gradientAxisEnd, D2D1_EXTEND_MODE extendMode, D2D1_GAMMA gamma) noexcept :
	ColorBrush(deviceResources),
	m_stops(stops),
	m_gradientAxisStart(gradientAxisStart),
	m_gradientAxisEnd(gradientAxisEnd),
	m_extendMode(extendMode),
	m_gamma(gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	Refresh();
}
GradientBrush::GradientBrush(const GradientBrush& rhs) noexcept :
	ColorBrush(rhs.m_deviceResources),
	m_stops(rhs.m_stops),
	m_gradientAxisStart(rhs.m_gradientAxisStart),
	m_gradientAxisEnd(rhs.m_gradientAxisEnd),
	m_extendMode(rhs.m_extendMode),
	m_gamma(rhs.m_gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	Refresh();
}
void GradientBrush::operator=(const GradientBrush& rhs) noexcept
{
	m_deviceResources = rhs.m_deviceResources;
	m_brush = nullptr;
	m_stops = rhs.m_stops;
	m_gradientAxisStart = rhs.m_gradientAxisStart;
	m_gradientAxisEnd = rhs.m_gradientAxisEnd;
	m_extendMode = rhs.m_extendMode;
	m_gamma = rhs.m_gamma;
	m_pGradientStops = nullptr;

	RefreshGradientStops();
	Refresh();
}

void GradientBrush::RefreshGradientStops() noexcept
{
	// Refresh() will be called when a layout/control changes size and will need to update
	// the gradient start/end. To avoid the unnecessary cost of recreating the gradient stops,
	// that functionality has been moved to this function.
	m_deviceResources->D2DDeviceContext()->CreateGradientStopCollection(
		m_stops.data(),
		static_cast<UINT32>(m_stops.size()),
		m_gamma,
		m_extendMode,
		m_pGradientStops.ReleaseAndGetAddressOf()
	);
}
void GradientBrush::Refresh() noexcept
{
	// Refresh will be called when a layout/control changes size and will need to update
	// the gradient start/end. To avoid the unnecessary cost of recreating the gradient stops,
	// that functionality has been moved out of this function.
	ComPtr<ID2D1LinearGradientBrush> gradientBrush = nullptr;

	m_deviceResources->D2DDeviceContext()->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(m_gradientAxisStart, m_gradientAxisEnd),
		m_pGradientStops.Get(),
		gradientBrush.ReleaseAndGetAddressOf()
	);

	gradientBrush->QueryInterface<ID2D1Brush>(m_brush.ReleaseAndGetAddressOf());
}
}