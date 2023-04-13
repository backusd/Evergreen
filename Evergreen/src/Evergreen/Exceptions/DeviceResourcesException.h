#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "BaseException.h"

namespace Evergreen
{
// NOTE: All exception classes MUST be defined in header files ONLY. This allows us to not have to __declspec(dllexport)
//       the class. This is important because you cannot DLL export std::exception. Therefore, doing it this way, it is
//		 up to the client code to supply and link to their implementation of std::exception, which is good because then 
//       there is no dependence on a specific standard library version/implementation.
class DeviceResourcesException : public BaseException
{
public:
	DeviceResourcesException(unsigned int line, const char* file) noexcept : BaseException(line, file)
	{}
	DeviceResourcesException(const DeviceResourcesException&) = delete;
	DeviceResourcesException& operator=(const DeviceResourcesException&) = delete;
	virtual ~DeviceResourcesException() noexcept override {}

	ND const char* GetType() const noexcept override
	{
		return "Device Resources Exception";
	}
};

// ================================================================================
// NOTE: All exception classes MUST be defined in header files ONLY. This allows us to not have to __declspec(dllexport)
//       the class. This is important because you cannot DLL export std::exception. Therefore, doing it this way, it is
//		 up to the client code to supply and link to their implementation of std::exception, which is good because then 
//       there is no dependence on a specific standard library version/implementation.
class InfoException : public BaseException
{
public:
	InfoException(int line, const char* file, const std::vector<std::string>& infoMsgs) noexcept :
		BaseException(line, file)
	{
		// join all info messages with newlines into single string
		for (const auto& m : infoMsgs)
		{
			m_info += m;
			m_info.push_back('\n');
		}
		// remove final newline if exists
		if (!m_info.empty())
		{
			m_info.pop_back();
		}
	}
	InfoException(const InfoException&) = delete;
	InfoException& operator=(const InfoException&) = delete;
	virtual ~InfoException() noexcept override {}

	ND const char* what() const noexcept override
	{
		m_whatBuffer = std::format("{}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorInfo(), GetOriginString());
		return m_whatBuffer.c_str();
	}
	ND const char* GetType() const noexcept override
	{
		return "Graphics Info Exception";
	}
	ND std::string GetErrorInfo() const noexcept
	{
		return m_info;
	}
private:
	std::string m_info;
};

}
