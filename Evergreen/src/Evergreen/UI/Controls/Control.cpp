#include "pch.h"
#include "Control.h"




namespace Evergreen
{
Control::Control(std::shared_ptr<DeviceResources> deviceResources, 
				 UI* ui,
				 const D2D1_RECT_F& allowedRegion,
	             const Evergreen::Margin& margin) noexcept :
	m_name(""),
	m_deviceResources(deviceResources),
	m_ui(ui),
	m_margin(margin),
	m_allowedRegion(allowedRegion),
	m_id(0)
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_ui != nullptr, "No UI");
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
