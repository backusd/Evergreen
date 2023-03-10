#pragma once
#include "pch.h"
#include "BaseException.h"

namespace Evergreen
{
class DeviceResourcesException : public BaseException
{
public:
	DeviceResourcesException(unsigned int line, const char* file) noexcept;
	DeviceResourcesException(const DeviceResourcesException&) = delete;
	void operator=(const DeviceResourcesException&) = delete;
	virtual ~DeviceResourcesException() noexcept override {}

	const char* GetType() const noexcept override;
};

// ================================================================================

class InfoException : public BaseException
{
public:
	InfoException(int line, const char* file, const std::vector<std::string>& infoMsgs) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;
private:
	std::string m_info;
};

}