#pragma once
#include "pch.h"
#include "BaseException.h"

namespace Evergreen
{
// NOTE: All exception classes MUST be defined in header files ONLY. This allows us to not have to __declspec(dllexport)
//       the class. This is important because you cannot DLL export std::exception. Therefore, doing it this way, it is
//		 up to the client code to supply and link to their implementation of std::exception, which is good because then 
//       there is no dependence on a specific standard library version/implementation.
class JSONLoadersException : public BaseException
{
public:
	JSONLoadersException(int line, const char* file, const std::string& message) noexcept :
		BaseException(line, file),
		m_message(message)
	{}
	JSONLoadersException(const JSONLoadersException&) = delete;
	void operator=(const JSONLoadersException&) = delete;
	virtual ~JSONLoadersException() noexcept override {}

	ND const char* what() const noexcept override
	{
		m_whatBuffer = std::format("{}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorInfo(), GetOriginString());
		return m_whatBuffer.c_str();
	}

	ND const char* GetType() const noexcept override
	{
		return "JSONLoaders Exception";
	}
	ND std::string GetErrorInfo() const noexcept
	{
		return m_message;
	}
private:
	std::string m_message;
};

}

#define JSON_LOADER_EXCEPTION(fmt, ...) throw Evergreen::JSONLoadersException(__LINE__, __FILE__, std::format(fmt, __VA_ARGS__))
#define JSON_LOADER_EXCEPTION_IF_FALSE(condition, fmt, ...) if (!(condition)) JSON_LOADER_EXCEPTION(fmt, __VA_ARGS__)