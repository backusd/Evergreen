#include "pch.h"
#include "Style.h"

namespace Evergreen
{
Style::Style(std::shared_ptr<DeviceResources> deviceResources, const std::string& name) noexcept :
	m_deviceResources(deviceResources), 
	m_name(name)
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}
Style::Style(const Style& style) noexcept :
	m_deviceResources(style.m_deviceResources), 
	m_name(style.m_name + "_copy")
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}
void Style::operator=(const Style& rhs) noexcept
{
	m_deviceResources = rhs.m_deviceResources;
	m_name = rhs.m_name + "_copy";

	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
}


}