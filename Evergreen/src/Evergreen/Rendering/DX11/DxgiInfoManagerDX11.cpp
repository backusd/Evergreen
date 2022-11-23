#include "pch.h"

#ifdef EG_DX11

#include "DxgiInfoManagerDX11.h"
#include "Evergreen/Core.h"
#include "Evergreen/Exceptions/WindowException.h"
#include "DeviceResourcesExceptionDX11.h"


#pragma comment(lib, "dxguid.lib")

namespace Evergreen
{
DxgiInfoManagerDX11::DxgiInfoManagerDX11() noexcept
{
	// define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// load the dll that contains the function DXGIGetDebugInterface
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		TERMINATE_ON_THROW(throw WINDOW_LAST_EXCEPT());
	}

	// get address of DXGIGetDebugInterface in dll
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	if (DxgiGetDebugInterface == nullptr)
	{
		TERMINATE_ON_THROW(throw WINDOW_LAST_EXCEPT());
	}

	DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&pDxgiInfoQueue));

	//TERMINATE_ON_THROW(GFX_THROW_NOINFO(
//		DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&pDxgiInfoQueue))
//	));
}

DxgiInfoManagerDX11::~DxgiInfoManagerDX11() noexcept
{
	if (pDxgiInfoQueue != nullptr)
	{
		pDxgiInfoQueue->Release();
	}
}

void DxgiInfoManagerDX11::Set() noexcept
{
	// set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManagerDX11::GetMessages() const noexcept
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		SIZE_T messageLength;
		// get the size of message i in bytes
		TERMINATE_ON_THROW(GFX_THROW_NOINFO(
			pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength)
		));
		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// get the message and push its description into the vector
		TERMINATE_ON_THROW(GFX_THROW_NOINFO(
			pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength)
		));
		//std::string s = pMessage->pDescription;
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}


}

#endif // EG_DX11