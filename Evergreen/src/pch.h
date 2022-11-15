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

#ifdef EG_PLATFORM_WINDOWS

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
#endif // EG_PLATFORM_WINDOWS

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
	#error DirectX12 not yet supported
#elif EG_OPENGL
	#error OpenGL not yet supported
#elif EG_VULKAN
	#error Vulkan not yet supported
#endif