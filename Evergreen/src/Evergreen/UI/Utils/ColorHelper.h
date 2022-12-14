#pragma once
#include "pch.h"
#include "Evergreen/Exceptions/BaseException.h"

namespace Evergreen
{
D2D1_COLOR_F ColorFromString(const std::string& colorName);



class ColorException : public BaseException
{
public:
	ColorException(int line, const char* file, const std::string& message) noexcept;
	ColorException(const ColorException&) = delete;
	void operator=(const ColorException&) = delete;
	virtual ~ColorException() noexcept override {}

	ND const char* what() const noexcept override;

	ND inline const char* GetType() const noexcept override;
	ND inline std::string GetErrorInfo() const noexcept;

private:
	std::string m_message;
};
}