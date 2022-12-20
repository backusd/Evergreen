#include "pch.h"
#include "JSONLoadersException.h"

namespace Evergreen
{
	JSONLoadersException::JSONLoadersException(int line, const char* file, const std::string& message) noexcept :
		BaseException(line, file),
		m_message(message)
	{
	}

	const char* JSONLoadersException::what() const noexcept
	{
		m_whatBuffer = std::format("{}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorInfo(), GetOriginString());
		return m_whatBuffer.c_str();
	}

	const char* JSONLoadersException::GetType() const noexcept
	{
		return "JSONLoaders Exception";
	}

	std::string JSONLoadersException::GetErrorInfo() const noexcept
	{
		return m_message;
	}

}