#pragma once
#ifdef EG_DX11

#include "pch.h"
#include "Evergreen/Exceptions/DeviceResourcesException.h"

namespace Evergreen
{
class DeviceResourcesExceptionDX11 : public DeviceResourcesException
{
public:
	DeviceResourcesExceptionDX11(unsigned int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
	DeviceResourcesExceptionDX11(const DeviceResourcesExceptionDX11&) = delete;
	void operator=(const DeviceResourcesExceptionDX11&) = delete;

	const char* what() const noexcept override;

	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorInfo() const noexcept;

	static std::string TranslateErrorCode(HRESULT hr) noexcept;

protected:
	HRESULT m_hr;
	std::string m_info;
};

// ================================================================================

class DeviceRemovedExceptionDX11 : public DeviceResourcesExceptionDX11
{
public:
	DeviceRemovedExceptionDX11(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
	const char* GetType() const noexcept override;
private:
	std::string m_reason;
};


}

#endif // EG_DX11