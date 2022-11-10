#include "pch.h"

#ifdef EG_DX11
#include "DeviceResourcesExceptionDX11.h"

namespace Evergreen
{
DeviceResourcesExceptionDX11::DeviceResourcesExceptionDX11(unsigned int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	DeviceResourcesException(line, file),
	m_hr(hr)
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

const char* DeviceResourcesExceptionDX11::what() const noexcept
{
	if(m_info.empty())
		m_whatBuffer = std::format("{}\n[Error Code] {:#x} ({})\n[Error String] {}\n{}", GetType(), GetErrorCode(), GetErrorCode(), TranslateErrorCode(m_hr), GetOriginString());
	else
		m_whatBuffer = std::format("{}\n[Error Code] {:#x} ({})\n[Error String] {}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorCode(), GetErrorCode(), TranslateErrorCode(m_hr), GetErrorInfo(), GetOriginString());

	return m_whatBuffer.c_str();
}

HRESULT DeviceResourcesExceptionDX11::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string DeviceResourcesExceptionDX11::GetErrorInfo() const noexcept
{
	return m_info;
}

std::string DeviceResourcesExceptionDX11::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}



// DeviceRemovedException ===========================================================================

DeviceRemovedExceptionDX11::DeviceRemovedExceptionDX11(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept :
	DeviceResourcesExceptionDX11(line, file, hr, infoMsgs)
{}


const char* DeviceRemovedExceptionDX11::GetType() const noexcept
{
	return "Device Resources Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}



}

#endif // EG_DX11