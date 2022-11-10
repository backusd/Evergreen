#include "pch.h"
#include "BaseException.h"


namespace Evergreen
{
BaseException::BaseException(int line, const char* file) noexcept :
	m_line(line),
	m_file(file)
{
}

const char* BaseException::what() const noexcept
{
	m_whatBuffer = std::format("{}\n{}", GetType(), GetOriginString());
	return m_whatBuffer.c_str();
}

const char* BaseException::GetType() const noexcept
{
	return "Base Exception";
}

int BaseException::GetLine() const noexcept
{
	return m_line;
}

const std::string& BaseException::GetFile() const noexcept
{
	return m_file;
}

std::string BaseException::GetOriginString() const noexcept
{
	return std::format("[File] {}\n[Line] {}", m_file, m_line);
}
}