#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <array>
#include <format>
#include <exception>
#include <optional>
#include <filesystem>
#include <algorithm>
#include <type_traits>


#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS	//--> Causes build to fail because WinMain.cpp uses SC_KEYMENU
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR			// --> Causes build to fail because imgui_impl_win32.cpp uses COLOR_BACKGROUND
#define NOCTLMGR		// --> Causes build to fail when #include <comdef.h> which is used for _com_error
#define NODRAWTEXT
#define NOKERNEL
// #define NONLS			// --> Causes build to fail - undeclared identifier
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define NOMINMAX

#define STRICT

#include <Windows.h>
#include <WinUser.h>

#ifdef EG_DX11

	#include <dxgidebug.h>

	#include <d2d1.h>
	#pragma comment(lib, "d2d1")
	#include <d2d1_3.h>
	#include <dwrite_3.h>
	#pragma comment(lib, "Dwrite")
	#include <wincodec.h>
	#include <comdef.h>
	#pragma comment(lib, "comsuppw")
	#include <d3d11_4.h>
	#pragma comment(lib, "D3D11")
	#include <wrl/client.h>
	#include <DirectXMath.h>
	#include <DirectXColors.h>
	#include <d3dcompiler.h>
	#pragma comment(lib, "D3DCompiler")

#elif EG_DX12

	// This include is NOT required, but does provide some Direct3d 12 helper structures
	// NOTE: "Note that these headers may conflict with the headers from the Windows SDK, 
	//        depending on include ordering. These headers should be added to the include 
	//		  directory list before the SDK, and should be included before other graphics 
	//		  headers (e.g. d3d11.h) from the Windows SDK. Otherwise, the corresponding 
	//	      header from the Windows SDK may be included first, and will define the include 
	//		  guards which prevents these headers from being used."
	// See: https://github.com/microsoft/DirectX-Headers
	#include <directx/d3dx12.h>

	// DXGI
	#include <dxgi.h>
	#include <dxgi1_2.h>
	#include <dxgi1_3.h>
	#include <dxgi1_4.h>
	#include <dxgi1_5.h>
	#include <dxgi1_6.h>
	#include <dxgicommon.h>
	#include <dxgidebug.h>
	#include <dxgiformat.h>

	// This is used for interop with D3D11, specifically, make use of Direct2D
	#include <d3d11on12.h>

	#include <d2d1.h>
	#pragma comment(lib, "d2d1")
	#include <d2d1_3.h>
	#include <dwrite_3.h>
	#pragma comment(lib, "Dwrite")
	#include <wincodec.h>
	#include <comdef.h>
	#pragma comment(lib, "comsuppw")
	#include <d3d11_4.h>
	#pragma comment(lib, "D3D11")
	#include <wrl/client.h>

//	#include <d3dcompiler.h>
//	#pragma comment(lib, "D3DCompiler")


	#include <d3d12.h>
	#include <D3Dcompiler.h>
	#include <DirectXMath.h>
	#include <DirectXPackedVector.h>
	#include <DirectXColors.h>
	#include <DirectXCollision.h>

	// Link necessary d3d12 libraries.
	#pragma comment(lib,"d3dcompiler.lib")
	#pragma comment(lib, "D3D12.lib")
	#pragma comment(lib, "dxgi.lib")

#elif EG_OPENGL
	#error OpenGL not yet supported
#elif EG_VULKAN
	#error Vulkan not yet supported
#endif