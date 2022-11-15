#include "pch.h"
#include "WindowsWindowException.h"

namespace Evergreen
{
WindowsWindowException::WindowsWindowException(unsigned int line, const char* file, HRESULT hr) noexcept :
	WindowException(line, file),
	hr(hr)
{
}

std::string WindowsWindowException::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),
		0,
		nullptr
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

const char* WindowsWindowException::what() const noexcept
{
	m_whatBuffer = std::format("{}\n[Error Code] {:#x} ({})\n[Description] {}\n{}", GetType(), GetErrorCode(), GetErrorCode(), GetErrorDescription(), GetOriginString());
	return m_whatBuffer.c_str();
}

HRESULT WindowsWindowException::GetErrorCode() const noexcept
{
	return hr;
}

std::string WindowsWindowException::GetErrorDescription() const noexcept
{
	return TranslateErrorCode(hr);
}

}