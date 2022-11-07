#include "pch.h"
#include "BaseException.h"
#include <sstream>


BaseException::BaseException(int line, const char* file) noexcept :
	m_line(line),
	m_file(file)
{
}

const char* BaseException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	m_whatBuffer = oss.str();
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
	std::ostringstream oss;
	oss << "[File] " << m_file << std::endl
		<< "[Line] " << m_line;
	return oss.str();
}
