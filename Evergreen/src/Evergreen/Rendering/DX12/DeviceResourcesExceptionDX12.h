#pragma once
#ifdef EG_DX12

#include "pch.h"
#include "Evergreen/Exceptions/DeviceResourcesException.h"

namespace Evergreen
{
class DeviceResourcesExceptionDX12 : public DeviceResourcesException
{
public:
	DeviceResourcesExceptionDX12(unsigned int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
	DeviceResourcesExceptionDX12(const DeviceResourcesExceptionDX12&) = delete;
	DeviceResourcesExceptionDX12& operator=(const DeviceResourcesExceptionDX12&) = delete;
	virtual ~DeviceResourcesExceptionDX12() noexcept override {}

	const char* what() const noexcept override;

	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorInfo() const noexcept;

	static std::string TranslateErrorCode(HRESULT hr) noexcept;

protected:
	HRESULT m_hr;
	std::string m_info;
};

// ================================================================================

class DeviceRemovedExceptionDX12 : public DeviceResourcesExceptionDX12
{
public:
	DeviceRemovedExceptionDX12(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
	const char* GetType() const noexcept override;
private:
	std::string m_reason;
};


}

#endif // EG_DX11