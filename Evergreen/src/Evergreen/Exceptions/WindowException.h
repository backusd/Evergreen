#pragma once
#include "pch.h"
#include "BaseException.h"
#include "Evergreen/Window/TranslateErrorCode.h"

namespace Evergreen
{
class WindowException : public BaseException
{
public:
	WindowException(unsigned int line, const char* file, HRESULT hr) noexcept;
	WindowException(const WindowException&) = delete;
	void operator=(const WindowException&) = delete;

	const char* GetType() const noexcept override;

	const char* what() const noexcept override;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorDescription() const noexcept;

private:
	HRESULT m_hr;
};


}