#pragma once
#include "pch.h"
#include "Evergreen/Exceptions/BaseException.h"

namespace Evergreen
{
D2D1_COLOR_F ColorFromString(const std::string& colorName);


// NOTE: All exception classes MUST be defined in header files ONLY. This allows us to not have to __declspec(dllexport)
//       the class. This is important because you cannot DLL export std::exception. Therefore, doing it this way, it is
//		 up to the client code to supply and link to their implementation of std::exception, which is good because then 
//       there is no dependence on a specific standard library version/implementation.
class ColorException : public BaseException
{
public:
	ColorException(int line, const char* file, const std::string& message) noexcept :
		BaseException(line, file),
		m_message(message)
	{}
	ColorException(const ColorException&) = delete;
	ColorException& operator=(const ColorException&) = delete;
	virtual ~ColorException() noexcept override {}

	ND const char* what() const noexcept override 
	{
		m_whatBuffer = std::format("{}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorInfo(), GetOriginString());
		return m_whatBuffer.c_str();
	}

	ND inline const char* GetType() const noexcept override
	{
		return "Color Exception";
	}
	ND inline std::string GetErrorInfo() const noexcept
	{
		return m_message;
	}

private:
	std::string m_message;
};
}