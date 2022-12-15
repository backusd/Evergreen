#include "pch.h"
#include "Control.h"




namespace Evergreen
{
Control::Control(std::shared_ptr<DeviceResources> deviceResources) noexcept :
	m_name(""),
	m_deviceResources(deviceResources),
	m_margin({ 0.0f, 0.0f, 0.0f, 0.0f }),
	m_allowedRegion({ 0.0f, 0.0f, FLT_MAX, FLT_MAX }),
	m_topLeftPosition({ 0.0f, 0.0f })
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}
Control::Control(const Control& control) noexcept :
	m_name(control.m_name + "_copy"),
	m_deviceResources(control.m_deviceResources),
	m_margin({ 0.0f, 0.0f, 0.0f, 0.0f }),
	m_allowedRegion({ 0.0f, 0.0f, FLT_MAX, FLT_MAX }),
	m_topLeftPosition({ 0.0f, 0.0f })
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}
void Control::operator=(const Control& control) noexcept 
{ 
	m_name = control.m_name + "_copy";
	m_deviceResources = control.m_deviceResources; 
	m_margin = control.m_margin;
	m_allowedRegion = control.m_allowedRegion;
	m_topLeftPosition = control.m_topLeftPosition;

	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}

void Control::Margin(float left, float top, float right, float bottom) noexcept
{
	m_margin.Left = left;
	m_margin.Top = top;
	m_margin.Right = right;
	m_margin.Bottom = bottom;

	OnMarginChanged();
}

void Control::AllowedRegion(float left, float top, float right, float bottom) noexcept
{
	m_allowedRegion.left = left;
	m_allowedRegion.top = top;
	m_allowedRegion.right = right;
	m_allowedRegion.bottom = bottom;

	OnAllowedRegionChanged();
}




}
