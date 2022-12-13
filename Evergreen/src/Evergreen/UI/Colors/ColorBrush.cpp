#include "pch.h"
#include "ColorBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
ColorBrush::ColorBrush(std::shared_ptr<DeviceResources> deviceResources, const D2D1_RECT_F& drawRegion) noexcept :
	m_deviceResources(deviceResources), m_brush(nullptr), 
	m_brushProperties{ 1.0f, D2D1::Matrix3x2F::Identity() },
	m_drawRegion(drawRegion)
{
	EG_CORE_ASSERT(deviceResources != nullptr, "No device resources");
}
ColorBrush::ColorBrush(const ColorBrush& rhs) noexcept :
	m_deviceResources(rhs.m_deviceResources), m_brush(nullptr), 
	m_brushProperties(rhs.m_brushProperties),
	m_drawRegion(rhs.m_drawRegion)
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}
void ColorBrush::operator=(const ColorBrush& rhs) noexcept
{
	EG_CORE_ASSERT(rhs.m_deviceResources != nullptr, "No device resources");

	m_deviceResources = rhs.m_deviceResources;
	m_brush = nullptr;
	m_brushProperties = rhs.m_brushProperties;
	m_drawRegion = rhs.m_drawRegion;
}


ID2D1Brush* ColorBrush::Get() const noexcept 
{ 
	// This should never be called if the brush has not been initialized
	EG_CORE_ASSERT(m_brush != nullptr, "brush has not been initialized");

	return m_brush.Get(); 
}
}