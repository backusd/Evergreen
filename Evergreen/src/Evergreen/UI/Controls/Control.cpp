#include "pch.h"
#include "Control.h"




namespace Evergreen
{
Control::Control(std::shared_ptr<DeviceResources> deviceResources) noexcept :
	m_deviceResources(deviceResources),
	m_margin({ 0.0f, 0.0f, 0.0f, 0.0f })
{
}
Control::Control(const Control& control) noexcept :
	m_deviceResources(control.m_deviceResources),
	m_margin({ 0.0f, 0.0f, 0.0f, 0.0f })
{
}
void Control::operator=(const Control& control) noexcept 
{ 
	m_deviceResources = control.m_deviceResources; 
	m_margin = control.m_margin;
}

void Control::Margin(float left, float top, float right, float bottom) noexcept
{
	m_margin.Left = left;
	m_margin.Top = top;
	m_margin.Right = right;
	m_margin.Bottom = bottom;
}


}
