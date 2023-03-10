#pragma once
#ifdef EG_DX12
#include "pch.h"


namespace Evergreen
{
class DxgiInfoManagerDX12
{
public:
	DxgiInfoManagerDX12();
	~DxgiInfoManagerDX12() noexcept;
	DxgiInfoManagerDX12(const DxgiInfoManagerDX12&) = delete;
	DxgiInfoManagerDX12& operator=(const DxgiInfoManagerDX12&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr;
};
}

#endif // EG_DX11