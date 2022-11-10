#include "pch.h"
#include "DeviceResourcesException.h"

namespace Evergreen
{
// DeviceResourcesException ===========================================================================
DeviceResourcesException::DeviceResourcesException(unsigned int line, const char* file) noexcept :
	BaseException(line, file)
{
}

const char* DeviceResourcesException::GetType() const noexcept
{
	return "Device Resources Exception";
}

// InfoException ====================================================================================
InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept :
	BaseException(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		m_info += m;
		m_info.push_back('\n');
	}
	// remove final newline if exists
	if (!m_info.empty())
	{
		m_info.pop_back();
	}
}


const char* InfoException::what() const noexcept
{
	m_whatBuffer = std::format("{}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorInfo(), GetOriginString());
	return m_whatBuffer.c_str();
}

const char* InfoException::GetType() const noexcept
{
	return "Graphics Info Exception";
}

std::string InfoException::GetErrorInfo() const noexcept
{
	return m_info;
}

}