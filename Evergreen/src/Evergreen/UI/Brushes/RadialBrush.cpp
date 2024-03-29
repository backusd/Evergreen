#include "pch.h"
#include "RadialBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
// This constructor is for a gradient with exactly 2 colors - 1 at the center and 1 at the edge
RadialBrush::RadialBrush(std::shared_ptr<DeviceResources> deviceResources, const D2D1_COLOR_F& startColor, 
	const D2D1_COLOR_F& endColor, const D2D1_POINT_2F& originOffset, D2D1_EXTEND_MODE extendMode, D2D1_GAMMA gamma) :
	ColorBrush(deviceResources),
	m_stops{ {0.0f, startColor}, {1.0f, endColor} },
	m_gradientOriginOffset(originOffset),
	m_extendMode(extendMode),
	m_gamma(gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	Refresh();
}
RadialBrush::RadialBrush(std::shared_ptr<DeviceResources> deviceResources, const std::vector<D2D1_GRADIENT_STOP>& stops, 
	const D2D1_POINT_2F& originOffset, D2D1_EXTEND_MODE extendMode, D2D1_GAMMA gamma) :
	ColorBrush(deviceResources),
	m_stops(stops),
	m_gradientOriginOffset(originOffset),
	m_extendMode(extendMode),
	m_gamma(gamma),
	m_pGradientStops(nullptr)
{
	// Disallow creating the brush without at least two stops
	EG_CORE_ASSERT(m_stops.size() >= 2, "Not enough stops supplied");

	RefreshGradientStops();
	Refresh();
}
RadialBrush::RadialBrush(const RadialBrush& rhs) :
	ColorBrush(rhs.m_deviceResources),
	m_stops(rhs.m_stops),
	m_gradientOriginOffset(rhs.m_gradientOriginOffset),
	m_extendMode(rhs.m_extendMode),
	m_gamma(rhs.m_gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	Refresh();
}
RadialBrush& RadialBrush::operator=(const RadialBrush& rhs)
{
	ColorBrush::operator=(rhs);

	m_stops = rhs.m_stops;
	m_gradientOriginOffset = rhs.m_gradientOriginOffset;
	m_extendMode = rhs.m_extendMode;
	m_gamma = rhs.m_gamma;
	m_pGradientStops = nullptr;

	RefreshGradientStops();
	Refresh();

	return *this;
}
std::unique_ptr<ColorBrush> RadialBrush::Duplicate()
{
	return std::move(std::make_unique<RadialBrush>(*this));
}
void RadialBrush::RefreshGradientStops()
{
	// Something went wrong if there are not at least two stops
	EG_CORE_ASSERT(m_stops.size() >= 2, "Not enough stops supplied");
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

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
void RadialBrush::Refresh()
{
	// gradient stops collection should have been created by now 
	EG_CORE_ASSERT(m_pGradientStops != nullptr, "gradient stops collection is nullptr");
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	// Refresh may need to be called when the mouse moves and will need to update
	// the brush. To avoid the unnecessary cost of recreating the gradient stops,
	// that functionality has been moved out of this function.
	ComPtr<ID2D1RadialGradientBrush> radialBrush = nullptr;
	GFX_THROW_INFO(
		m_deviceResources->D2DDeviceContext()->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(
				Center(),
				m_gradientOriginOffset,
				RadiusX(),
				RadiusY()),
			m_brushProperties,
			m_pGradientStops.Get(),
			radialBrush.ReleaseAndGetAddressOf()
		)
	)

	GFX_THROW_INFO(
		radialBrush->QueryInterface<ID2D1Brush>(m_brush.ReleaseAndGetAddressOf())
	)
}

void RadialBrush::Stops(const std::vector<D2D1_GRADIENT_STOP>& stops)
{
	// Disallow creating the brush without at least two stops
	EG_CORE_ASSERT(stops.size() >= 2, "Not enough stops supplied");

	m_stops = stops;
	RefreshGradientStops();
	Refresh();
}
void RadialBrush::Gamma(D2D1_GAMMA gamma) 
{ 
	m_gamma = gamma; 
	RefreshGradientStops(); 
	Refresh();
}
void RadialBrush::ExtendMode(D2D1_EXTEND_MODE mode) 
{ 
	m_extendMode = mode; 
	RefreshGradientStops(); 
	Refresh();
}
void RadialBrush::GradientOriginOffset(const D2D1_POINT_2F& offset) 
{ 
	m_gradientOriginOffset = offset; 
	Refresh(); 
}



}