#pragma once
#ifdef EG_DX11
#include "pch.h"


namespace Evergreen
{
class DxgiInfoManagerDX11
{
public:
	DxgiInfoManagerDX11();
	~DxgiInfoManagerDX11() noexcept;
	DxgiInfoManagerDX11(const DxgiInfoManagerDX11&) = delete;
	DxgiInfoManagerDX11& operator=(const DxgiInfoManagerDX11&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr;
};
}

#endif // EG_DX11