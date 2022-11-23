#include "pch.h"
#include "WindowException.h"

namespace Evergreen
{
WindowException::WindowException(unsigned int line, const char* file, HRESULT hr) noexcept :
	BaseException(line, file),
	m_hr(hr)
{
}

const char* WindowException::GetType() const noexcept
{
	return "Window Exception";
}

const char* WindowException::what() const noexcept
{
	m_whatBuffer = std::format("{}\n[Error Code] {:#x} ({})\n[Description] {}\n{}", GetType(), GetErrorCode(), GetErrorCode(), GetErrorDescription(), GetOriginString());
	return m_whatBuffer.c_str();
}

HRESULT WindowException::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string WindowException::GetErrorDescription() const noexcept
{
	return TranslateErrorCode(m_hr);
}


}