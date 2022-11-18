#include "pch.h"
#include "WindowsWindowException.h"
#include "TranslateErrorCode.h"

namespace Evergreen
{
WindowsWindowException::WindowsWindowException(unsigned int line, const char* file, HRESULT hr) noexcept :
	WindowException(line, file),
	hr(hr)
{
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