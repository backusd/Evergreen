#pragma once
#ifdef EG_DX11
#include "pch.h"
#include "Evergreen/Core.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API DxgiInfoManagerDX11
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
#pragma warning( pop )
}

#endif // EG_DX11