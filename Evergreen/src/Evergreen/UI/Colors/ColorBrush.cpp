#include "pch.h"
#include "ColorBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
ColorBrush::ColorBrush(std::shared_ptr<DeviceResources> deviceResources) noexcept :
	m_deviceResources(deviceResources), m_brush(nullptr), 
	m_brushProperties{ 1.0f, D2D1::Matrix3x2F::Identity() }
{
}
ColorBrush::ColorBrush(const ColorBrush& rhs) noexcept :
	m_deviceResources(rhs.m_deviceResources), m_brush(nullptr), 
	m_brushProperties(rhs.m_brushProperties)
{
}
void ColorBrush::operator=(const ColorBrush& rhs) noexcept
{
	m_deviceResources = rhs.m_deviceResources;
	m_brush = nullptr;
	m_brushProperties = rhs.m_brushProperties;
}



}