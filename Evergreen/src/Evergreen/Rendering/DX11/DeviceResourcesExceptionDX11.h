#pragma once
#ifdef EG_DX11

#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Exceptions/DeviceResourcesException.h"

namespace Evergreen
{
// NOTE: All exception classes MUST be defined in header files ONLY. This allows us to not have to __declspec(dllexport)
//       the class. This is important because you cannot DLL export std::exception. Therefore, doing it this way, it is
//		 up to the client code to supply and link to their implementation of std::exception, which is good because then 
//       there is no dependence on a specific standard library version/implementation.
class DeviceResourcesExceptionDX11 : public DeviceResourcesException
{
public:
	DeviceResourcesExceptionDX11(unsigned int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept :
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
	DeviceResourcesExceptionDX11(const DeviceResourcesExceptionDX11&) = delete;
	void operator=(const DeviceResourcesExceptionDX11&) = delete;
	virtual ~DeviceResourcesExceptionDX11() noexcept override {}

	ND const char* what() const noexcept override
	{
		if (m_info.empty())
			m_whatBuffer = std::format("{}\n[Error Code] {:#x} ({})\n[Error String] {}\n{}", GetType(), GetErrorCode(), GetErrorCode(), TranslateErrorCode(m_hr), GetOriginString());
		else
			m_whatBuffer = std::format("{}\n[Error Code] {:#x} ({})\n[Error String] {}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorCode(), GetErrorCode(), TranslateErrorCode(m_hr), GetErrorInfo(), GetOriginString());

		return m_whatBuffer.c_str();
	}

	ND HRESULT GetErrorCode() const noexcept
	{
		return m_hr;
	}
	ND std::string GetErrorInfo() const noexcept
	{
		return m_info;
	}

	ND static std::string TranslateErrorCode(HRESULT hr) noexcept
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

protected:
	HRESULT m_hr;
	std::string m_info;
};

// ================================================================================
// NOTE: All exception classes MUST be defined in header files ONLY. This allows us to not have to __declspec(dllexport)
//       the class. This is important because you cannot DLL export std::exception. Therefore, doing it this way, it is
//		 up to the client code to supply and link to their implementation of std::exception, which is good because then 
//       there is no dependence on a specific standard library version/implementation.
class DeviceRemovedExceptionDX11 : public DeviceResourcesExceptionDX11
{
public:
	DeviceRemovedExceptionDX11(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept :
		DeviceResourcesExceptionDX11(line, file, hr, infoMsgs)
	{}
	ND const char* GetType() const noexcept override
	{
		return "Device Resources Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
	}
private:
	std::string m_reason;
};

}

#endif // EG_DX11