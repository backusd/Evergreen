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
	m_deviceResources(rhs.m_deviceResources), 
	m_brush(nullptr), 
	m_brushProperties(rhs.m_brushProperties),
	m_drawRegion(rhs.m_drawRegion),
	m_DrawRegionRectModifier(rhs.m_DrawRegionRectModifier)
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}
void ColorBrush::operator=(const ColorBrush& rhs) noexcept
{
	m_deviceResources = rhs.m_deviceResources;
	m_brush = nullptr;
	m_brushProperties = rhs.m_brushProperties;
	m_drawRegion = rhs.m_drawRegion;
	m_DrawRegionRectModifier = rhs.m_DrawRegionRectModifier;

	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}

ID2D1Brush* ColorBrush::Get() const noexcept 
{ 
	// This should never be called if the brush has not been initialized
	EG_CORE_ASSERT(m_brush != nullptr, "brush has not been initialized");
	return m_brush.Get(); 
}

void ColorBrush::SetDrawRegion(const D2D1_RECT_F& rect) noexcept
{ 
	// Call the modifier functional which may or mar not modify the rect to set as the draw region
	m_drawRegion = m_DrawRegionRectModifier(rect, this);
	OnDrawRegionChanged(); 
}
void ColorBrush::SetOpacity(float opacity) noexcept 
{ 
	m_brushProperties.opacity = opacity; 
	Refresh(); 
}
void ColorBrush::SetTransform(const D2D1_MATRIX_3X2_F& transform) noexcept 
{ 
	m_brushProperties.transform = transform; 
	Refresh(); 
}

}