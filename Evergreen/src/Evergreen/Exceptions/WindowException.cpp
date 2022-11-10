#include "pch.h"
#include "WindowException.h"

namespace Evergreen
{
WindowException::WindowException(unsigned int line, const char* file) noexcept :
	BaseException(line, file)
{
}

const char* WindowException::GetType() const noexcept
{
	return "Window Exception";
}
}