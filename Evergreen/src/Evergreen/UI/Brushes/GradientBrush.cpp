#include "pch.h"
#include "GradientBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
// This constructor is for a gradient with exactly 2 colors - 1 at the beginning and 1 at the end
GradientBrush::GradientBrush(std::shared_ptr<DeviceResources> deviceResources, const D2D1_COLOR_F& startColor, const D2D1_COLOR_F& endColor, 
	GRADIENT_AXIS axis, D2D1_EXTEND_MODE extendMode, D2D1_GAMMA gamma) :
	ColorBrush(deviceResources),
	m_stops{{0.0f, startColor}, {1.0f, endColor}},
	m_axis(axis),
	m_gradientAxisStart(D2D1::Point2F(500.0f, 0.0f)),
	m_gradientAxisEnd(D2D1::Point2F(500.0f, 1000.0f)),
	m_extendMode(extendMode),
	m_gamma(gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	RefreshGradientAxis();
	Refresh();
}
GradientBrush::GradientBrush(std::shared_ptr<DeviceResources> deviceResources, const std::vector<D2D1_GRADIENT_STOP>& stops,
	GRADIENT_AXIS axis, D2D1_EXTEND_MODE extendMode, D2D1_GAMMA gamma) :
	ColorBrush(deviceResources),
	m_stops(stops),
	m_axis(axis),
	m_gradientAxisStart(D2D1::Point2F(500.0f, 0.0f)),
	m_gradientAxisEnd(D2D1::Point2F(500.0f, 1000.0f)),
	m_extendMode(extendMode),
	m_gamma(gamma),
	m_pGradientStops(nullptr)
{
	// Disallow creating the brush without at least two stops
	EG_CORE_ASSERT(m_stops.size() >= 2, "Not enough stops supplied");

	RefreshGradientStops();
	RefreshGradientAxis();
	Refresh();
}
GradientBrush::GradientBrush(const GradientBrush& rhs) :
	ColorBrush(rhs.m_deviceResources),
	m_stops(rhs.m_stops),
	m_axis(rhs.m_axis),
	m_gradientAxisStart(rhs.m_gradientAxisStart),
	m_gradientAxisEnd(rhs.m_gradientAxisEnd),
	m_extendMode(rhs.m_extendMode),
	m_gamma(rhs.m_gamma),
	m_pGradientStops(nullptr)
{
	RefreshGradientStops();
	// RefreshGradientAxis(); <-- No need to do this because copying the gradient axis start/end should already be accurate
	Refresh();
}
void GradientBrush::operator=(const GradientBrush& rhs)
{
	ColorBrush::operator=(rhs);

	m_stops = rhs.m_stops;
	m_axis = rhs.m_axis;
	m_gradientAxisStart = rhs.m_gradientAxisStart;
	m_gradientAxisEnd = rhs.m_gradientAxisEnd;
	m_extendMode = rhs.m_extendMode;
	m_gamma = rhs.m_gamma;
	m_pGradientStops = nullptr;

	RefreshGradientStops();
	// RefreshGradientAxis(); <-- No need to do this because copying the gradient axis start/end should already be accurate
	Refresh();
}
std::unique_ptr<ColorBrush> GradientBrush::Duplicate()
{
	return std::move(std::make_unique<GradientBrush>(*this));
}
void GradientBrush::RefreshGradientAxis() noexcept
{
	switch (m_axis)
	{
	case GRADIENT_AXIS::HORIZONTAL:
		m_gradientAxisStart.x = m_drawRegion.left;
		m_gradientAxisStart.y = m_drawRegion.top + ((m_drawRegion.bottom - m_drawRegion.top) / 2.0f);

		m_gradientAxisEnd.x = m_drawRegion.right;
		m_gradientAxisEnd.y = m_gradientAxisStart.y;
		break;

	case GRADIENT_AXIS::VERTICAL:
		m_gradientAxisStart.x = m_drawRegion.left + ((m_drawRegion.right - m_drawRegion.left) / 2.0f);
		m_gradientAxisStart.y = m_drawRegion.top;

		m_gradientAxisEnd.x = m_gradientAxisStart.x;
		m_gradientAxisEnd.y = m_drawRegion.bottom;
		break;

	case GRADIENT_AXIS::NESW:
		m_gradientAxisStart.x = m_drawRegion.right;
		m_gradientAxisStart.y = m_drawRegion.top;

		m_gradientAxisEnd.x = m_drawRegion.left;
		m_gradientAxisEnd.y = m_drawRegion.bottom;
		break;

	case GRADIENT_AXIS::NWSE:
		m_gradientAxisStart.x = m_drawRegion.left;
		m_gradientAxisStart.y = m_drawRegion.top;

		m_gradientAxisEnd.x = m_drawRegion.right;
		m_gradientAxisEnd.y = m_drawRegion.bottom;
		break;
	}
}
void GradientBrush::RefreshGradientStops()
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
void GradientBrush::Refresh()
{
	// gradient stops collection should have been created by now 
	EG_CORE_ASSERT(m_pGradientStops != nullptr, "gradient stops collection is nullptr");
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	// Refresh will be called when a layout/control changes size and will need to update
	// the gradient start/end. To avoid the unnecessary cost of recreating the gradient stops,
	// that functionality has been moved out of this function.
	ComPtr<ID2D1LinearGradientBrush> gradientBrush = nullptr;

	GFX_THROW_INFO(
		m_deviceResources->D2DDeviceContext()->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(m_gradientAxisStart, m_gradientAxisEnd),
			m_brushProperties,
			m_pGradientStops.Get(),
			gradientBrush.ReleaseAndGetAddressOf()
		)
	)

	GFX_THROW_INFO(
		gradientBrush->QueryInterface<ID2D1Brush>(m_brush.ReleaseAndGetAddressOf())
	)
}

void GradientBrush::Stops(const std::vector<D2D1_GRADIENT_STOP>& stops)
{ 
	// Disallow creating the brush without at least two stops
	EG_CORE_ASSERT(stops.size() >= 2, "Not enough stops supplied");

	m_stops = stops;
	RefreshGradientStops(); 
	Refresh();
}
void GradientBrush::Gamma(D2D1_GAMMA gamma) 
{ 
	m_gamma = gamma; 
	RefreshGradientStops(); 
	Refresh();
}
void GradientBrush::ExtendMode(D2D1_EXTEND_MODE mode) 
{ 
	m_extendMode = mode; 
	RefreshGradientStops(); 
	Refresh();
}
void GradientBrush::GradientAxis(GRADIENT_AXIS axis) 
{ 
	m_axis = axis; 
	RefreshGradientAxis(); 
	Refresh();
}




}