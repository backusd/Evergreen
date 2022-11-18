#pragma once
#include "pch.h"
#include "Evergreen/Exceptions/WindowException.h"

namespace Evergreen
{
	class WindowsWindowException : public WindowException
	{
	public:
		WindowsWindowException(unsigned int line, const char* file, HRESULT hr) noexcept;
		WindowsWindowException(const WindowsWindowException&) = delete;
		void operator=(const WindowsWindowException&) = delete;

		const char* what() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;

	private:
		HRESULT hr;
	};
}