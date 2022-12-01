#include "pch.h"
#include "ColorBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
ColorBrush::ColorBrush(std::shared_ptr<DeviceResources> deviceResources) noexcept :
	m_deviceResources(deviceResources), m_brush(nullptr)
{
}
ColorBrush::ColorBrush(const ColorBrush& rhs) noexcept :
	m_deviceResources(rhs.m_deviceResources), m_brush(nullptr)
{
}
void ColorBrush::operator=(const ColorBrush& rhs) noexcept
{
	m_deviceResources = rhs.m_deviceResources;
	m_brush = nullptr;
}

}