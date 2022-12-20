#pragma once
#include "pch.h"
#include "Evergreen/Core.h"

namespace Evergreen
{
class BaseException : public std::exception
{
public:
	BaseException(int line, const char* file) noexcept;
	BaseException(const BaseException&) = delete;
	void operator=(const BaseException&) = delete;
	virtual ~BaseException() noexcept {}

	const char* what() const noexcept override;

	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int m_line;
	std::string m_file;

protected:
	mutable std::string m_whatBuffer;
};
}