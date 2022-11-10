#pragma once
#include "pch.h"
#include "BaseException.h"

namespace Evergreen
{
class WindowException : public BaseException
{
public:
	WindowException(unsigned int line, const char* file) noexcept;
	WindowException(const WindowException&) = delete;
	void operator=(const WindowException&) = delete;

	const char* GetType() const noexcept override;
};
}