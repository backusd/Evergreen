#pragma once
#include "pch.h"
#include "BaseException.h"

namespace Evergreen
{
class JSONLoadersException : public BaseException
{
public:
	JSONLoadersException(int line, const char* file, const std::string& message) noexcept;
	JSONLoadersException(const JSONLoadersException&) = delete;
	void operator=(const JSONLoadersException&) = delete;
	virtual ~JSONLoadersException() noexcept override {}

	const char* what() const noexcept override;

	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;

private:
	std::string m_message;
};

}

#define JSON_LOADER_EXCEPTION(fmt, ...) throw Evergreen::JSONLoadersException(__LINE__, __FILE__, std::format(fmt, __VA_ARGS__))
#define JSON_LOADER_EXCEPTION_IF_FALSE(condition, fmt, ...) if (!(condition)) JSON_LOADER_EXCEPTION(fmt, __VA_ARGS__)