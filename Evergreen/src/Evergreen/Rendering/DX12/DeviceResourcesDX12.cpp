#include "pch.h"
#ifdef EG_DX12

#include "DeviceResourcesDX12.h"
#include "DeviceResourcesExceptionDX12.h"
#include "DX12Helper.h"
#include "Evergreen/Core.h"
#include "Evergreen/Window/Window.h"
#include "Evergreen/Exceptions/WindowException.h"

using Microsoft::WRL::ComPtr;

#if defined(_DEBUG)
Evergreen::DxgiInfoManagerDX12 Evergreen::DeviceResourcesDX12::m_infoManager = Evergreen::DxgiInfoManagerDX12();
#endif

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
	pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
	WCHAR fullName[50];
	if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
	{
		pObject->SetName(fullName);
	}
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

namespace Evergreen
{
DeviceResourcesDX12::DeviceResourcesDX12(Window* window) :
	m_hWnd(window->GetHWND()),
	m_fenceValue(0),
	m_frameIndex(0),
	m_enableGPUTimeout(true),
	m_d3d11DeviceFlags(0),
	m_d2dFactoryOptions{},
	m_aspectRatio(static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight())),
	m_scissorRect(CD3DX12_RECT(0, 0, window->GetWidth(), window->GetHeight()))
{
	// Must initialize COM library
	GFX_THROW_INFO(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))

	CreateDXGIFactoryAndD3D12Device();
	// CheckMultiSamplingSupport(); <-- Can omit this for now. D3D12 does not support multisampling (see comment in DeviceResourcesDX12.h)
	CreateCommandObjects();
	CreateSwapChain(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));
	CreateD3D11On12Device();
	CreateD2DComponents();
	CreateRtvAndDsvDescriptorHeaps();
	CreateFence();

	OnResize(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));
}
void DeviceResourcesDX12::CreateDXGIFactoryAndD3D12Device()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			m_d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
			// m_d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUGGABLE; TODO: Research this further - causes device creation failure
			m_d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
		}
		else
		{
			EG_CORE_WARN("{}:{} - Build was in DEBUG, but call to D3D12GetDebugInterface failed", __FILE__, __LINE__);
		}
	}
#endif

	// NOTE: CreateDXGIFactory2 is the same as CreateDXGIFactory1, with the exception that it accepts 
	//       a 'flags' field which should either be 0 or DXGI_CREATE_FACTORY_DEBUG
	GFX_THROW_INFO(
		CreateDXGIFactory2(
			dxgiFactoryFlags,
			IID_PPV_ARGS(&m_dxgiFactory))
	);

	// NOTE: D3D12CreateDevice() will use the 1st hardware adapter it can find if nullptr is passed
	//       as the first parameter. This is fine, if the 1st adapter happens to support D3D12, but
	//       it is better to instead search for a D3D12 compatible adapter
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	GetHardwareAdapter(m_dxgiFactory.Get(), &hardwareAdapter);

	HRESULT hardwareResult = D3D12CreateDevice(
		hardwareAdapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_d3d12Device)
	);
	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		EG_CORE_WARN("{}:{} - Failed to create D3D12 Device. Falling back on WARP device.", __FILE__, __LINE__);

		ComPtr<IDXGIAdapter> warpAdapter;
		GFX_THROW_INFO(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		GFX_THROW_INFO(D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_d3d12Device)
		));
	}

#if defined(_DEBUG)
	// Filter a debug error coming from the 11on12 layer.
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(m_d3d12Device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		// Suppress whole categories of messages.
		//D3D12_MESSAGE_CATEGORY categories[] = {};

		// Suppress messages based on their severity level.
		D3D12_MESSAGE_SEVERITY severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO,
		};

		// Suppress individual messages by their ID.
		D3D12_MESSAGE_ID denyIds[] =
		{
			// This occurs when there are uninitialized descriptors in a descriptor table, even when a
			// shader does not access the missing descriptors.
			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

			// Not sure why this is occurring, but it is okay to ignore it for now
			// (not sure how to get rid of it because we don't (can't ?) specify a clear value when
			//  the renfer target is created)
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
		};

		D3D12_INFO_QUEUE_FILTER filter = {};
		//filter.DenyList.NumCategories = _countof(categories);
		//filter.DenyList.pCategoryList = categories;
		//filter.DenyList.NumSeverities = _countof(severities);
		//filter.DenyList.pSeverityList = severities;
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;

		GFX_THROW_INFO(infoQueue->PushStorageFilter(&filter));
	}
#endif
}
// Can omit this function for now. D3D12 does not support multisampling (see comment in DeviceResourcesDX12.h)
//void DeviceResourcesDX12::CheckMultiSamplingSupport()
//{
//	// Check 4X MSAA quality support for our back buffer format.
//	// All Direct3D 11 capable devices support 4X MSAA for all render 
//	// target formats, so we only need to check quality support.
//	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
//	msQualityLevels.Format = m_backBufferFormat;
//	msQualityLevels.SampleCount = 4;
//	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
//	msQualityLevels.NumQualityLevels = 0;
//	GFX_THROW_INFO(
//		m_d3d12Device->CheckFeatureSupport(
//			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
//			&msQualityLevels,
//			sizeof(msQualityLevels)
//		)
//	);
//
//	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
//	EG_CORE_ASSERT(m_4xMsaaQuality > 0, "Unexpected MSAA quality level.");
//}
void DeviceResourcesDX12::CreateCommandObjects()
{
	// ======================================================================
	// Command Queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // Type: Direct - "Specifies a command buffer that the GPU can execute. A direct command list doesn't inherit any GPU state."
	// Priority: Can be "Normal", "High", or "Global Realtime"
	//     "An application must be sufficiently privileged in order to create a command queue that has global realtime priority. If the application is not sufficiently 
	//      privileged or if neither the adapter or driver can provide the necessary preemption, then requests to create a global realtime priority queue fail; such a 
	//      failure could be due to a lack of hardware support or due to conflicts with other command queue parameters. Requests to create a global realtime command queue 
	//      won't silently downgrade the priority when it can't be supported; the request succeeds or fails as-is to indicate to the application whether or not the command 
	//      queue is guaranteed to execute before any other queue."
	// TODO: Test different priorities
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = m_enableGPUTimeout ? D3D12_COMMAND_QUEUE_FLAG_NONE : D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
	queueDesc.NodeMask = 0;	// Always 0 for single GPU systems

	GFX_THROW_INFO(m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	NAME_D3D12_OBJECT(m_commandQueue);

	// ======================================================================
	// Command Allocator
	GFX_THROW_INFO(
		m_d3d12Device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			IID_PPV_ARGS(&m_commandAllocator)
		)
	);
	NAME_D3D12_OBJECT(m_commandAllocator);

	// ======================================================================
	// Command List
	GFX_THROW_INFO(
		m_d3d12Device->CreateCommandList(
			0,								// NodeMask: "For single-GPU operation, set this to zero."
			D3D12_COMMAND_LIST_TYPE_DIRECT,	// Type: Direct = "Specifies a command buffer that the GPU can execute. A direct command list doesn't inherit any GPU state."
			m_commandAllocator.Get(),		// Command Allocator: "A pointer to the command allocator object from which the device creates command lists."
			nullptr,						// PipelineState: "If it is nullptr, then the runtime sets a dummy initial pipeline state, so that drivers don't have to deal with undefined state."
			IID_PPV_ARGS(&m_commandList)
		)
	);
	NAME_D3D12_OBJECT(m_commandList);

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	GFX_THROW_INFO(m_commandList->Close());
}
void DeviceResourcesDX12::CreateSwapChain(float width, float height)
{
	// Release the previous swapchain we will be recreating.
	m_swapChain.Reset();

	// Describe the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = static_cast<UINT>(width);
	swapChainDesc.Height = static_cast<UINT>(height);
	swapChainDesc.Format = m_backBufferFormat;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH; // "Directs DXGI to make the back-buffer contents scale to fit the presentation target size. This is the implicit behavior of DXGI"
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // This is the default. "Indicates that the transparency behavior is not specified."
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//swapChainDesc.Stereo = false; // TODO: Do more research on this. We already use a "flip-model swap chain" which is required, but what actually is this setting?
	// NOTE: These next two are only relevant for 'bitblt' model swap chains (We are using DXGI_SWAP_EFFECT_FLIP_DISCARD)
	swapChainDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1; // TODO: Try commenting these out as they should not be needed, but somehow cause the swap chain creation to fail
	swapChainDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
	fullscreenDesc.RefreshRate.Numerator = 60;
	fullscreenDesc.RefreshRate.Denominator = 1;
	fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	fullscreenDesc.Windowed = true;

	ComPtr<IDXGISwapChain1> swapChain;
	GFX_THROW_INFO(m_dxgiFactory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),	// Swap chain needs the queue so that it can force a flush on it.
		m_hWnd,
		&swapChainDesc,
		&fullscreenDesc,
		nullptr,				// Pointer to IDXGIOutput for the output to restrict content to. "Set this parameter to NULL if you don't want to restrict content to an output target."
		&swapChain
	));

	// This sample does not support fullscreen transitions.
	// GFX_THROW_INFO(m_dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

	GFX_THROW_INFO(swapChain.As(&m_swapChain));

	DXGI_RGBA backgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	GFX_THROW_INFO(m_swapChain->SetBackgroundColor(&backgroundColor));

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}
void DeviceResourcesDX12::CreateD3D11On12Device()
{
	// D3D11_CREATE_DEVICE_BGRA_SUPPORT: "Creates a device that supports BGRA formats (DXGI_FORMAT_B8G8R8A8_UNORM and
	//                                    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB). Required for Direct2D interoperability with Direct3D resources."
	m_d3d11DeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	// See comment in DeviceResourcesDX12.h about GPU timeout
	if (!m_enableGPUTimeout)
		m_d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT;

	// Create an 11 device wrapped around the 12 device and share
	// 12's command queue.
	ComPtr<ID3D11Device> d3d11Device;
	GFX_THROW_INFO(D3D11On12CreateDevice(
		m_d3d12Device.Get(),	// Pre-existing D3D12 device
		m_d3d11DeviceFlags,		// Flags to enable/disable different layers
		nullptr,				// FeatureLevelArray: "Providing NULL will default to the Direct3D 12 device's feature level."
		0,						// FeatureLevelArray Count
		reinterpret_cast<IUnknown**>(m_commandQueue.GetAddressOf()), // CommandQueues Array
		1,						// Size of CommandQueues Array
		0,						// NodeMask: For single GPU system, this should always be 0
		&d3d11Device,			// Pointer to returned D3D11Device
		&m_d3d11DeviceContext,	// Pointer to returned D3D11DeviceContext
		nullptr					// Pointer to the returned Feature Level (can be nullptr)
	));

	// Query the 11On12 device from the 11 device.
	GFX_THROW_INFO(d3d11Device.As(&m_d3d11On12Device));
}
void DeviceResourcesDX12::CreateD2DComponents()
{
	// Create D2D/DWrite components.
	GFX_THROW_INFO(
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory3),
			&m_d2dFactoryOptions,
			&m_d2dFactory
		)
	);

	// TODO: Test out D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS
	D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE; 
	ComPtr<IDXGIDevice> dxgiDevice;
	GFX_THROW_INFO(m_d3d11On12Device.As(&dxgiDevice));
	GFX_THROW_INFO(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
	GFX_THROW_INFO(m_d2dDevice->CreateDeviceContext(deviceOptions, &m_d2dDeviceContext));
	GFX_THROW_INFO(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dWriteFactory));

	GFX_THROW_INFO(
		CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(m_wicImagingFactory.ReleaseAndGetAddressOf())
		)
	);
}
void DeviceResourcesDX12::CreateRtvAndDsvDescriptorHeaps()
{
	m_rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Render Target View
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// Always no flags for RTV heap
	rtvHeapDesc.NodeMask = 0;								// Always 0 in a single GPU system
	GFX_THROW_INFO(m_d3d12Device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.ReleaseAndGetAddressOf())
	));
	NAME_D3D12_OBJECT(m_rtvHeap);

	// Depth Stencil View
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// Always no flags for DSV heap
	dsvHeapDesc.NodeMask = 0;								// Always 0 in a single GPU system
	GFX_THROW_INFO(m_d3d12Device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.ReleaseAndGetAddressOf())
	));
	NAME_D3D12_OBJECT(m_dsvHeap);
}
void DeviceResourcesDX12::CreateFence()
{
	GFX_THROW_INFO(
		m_d3d12Device->CreateFence(
			m_fenceValue,						// Initial fence value
			D3D12_FENCE_FLAG_NONE,				// Flags
			IID_PPV_ARGS(m_fence.GetAddressOf())
		)
	);
	NAME_D3D12_OBJECT(m_fence);
}

void DeviceResourcesDX12::OnResize(float width, float height)
{
	EG_CORE_ASSERT(m_d3d12Device != nullptr, "No D3D12 device");
	EG_CORE_ASSERT(m_swapChain != nullptr, "No swap chain");
	EG_CORE_ASSERT(m_commandList != nullptr, "No command list");

	m_commandList->OMSetRenderTargets(0, nullptr, true, nullptr);

	//m_d3dRenderTargetView = nullptr;
	m_d2dDeviceContext->SetTarget(nullptr);
	//m_d2dBitmap = nullptr;
	//m_d3dDepthStencilView = nullptr;
	//m_d3dDeviceContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	// Flush before changing any resources.
	FlushCommandQueue();

	GFX_THROW_INFO(
		m_commandList->Reset(
			m_commandAllocator.Get(),	// Pointer to command allocator
			nullptr						// Pointer to Pipeline State Object
		)
	);

	// Release the previous resources we will be recreating.
	for (int i = 0; i < FrameCount; ++i)
	{
		m_renderTargets[i].Reset();
		m_wrappedBackBuffers[i].Reset();
		m_d2dRenderTargets[i].Reset();
	}
	m_depthStencilBuffer.Reset();

//	ComPtr<IDXGIDebug> debug;
//	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
//	{
//		GFX_THROW_INFO(debug->ReportLiveObjects(DXGI_DEBUG_DX, DXGI_DEBUG_RLO_SUMMARY));
//	}

	m_d2dFactory = nullptr;
	m_d2dDevice = nullptr;
	m_d2dDeviceContext = nullptr;
	m_d2dFactory = nullptr;
	m_dWriteFactory = nullptr;
	m_wicImagingFactory = nullptr;

	m_d3d11DeviceContext = nullptr;
	m_d3d11On12Device = nullptr;

	// Resize the swap chain.
	GFX_THROW_INFO(
		m_swapChain->ResizeBuffers(
			FrameCount,
			static_cast<UINT>(width),
			static_cast<UINT>(height),
			m_backBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		)
	);

	CreateD3D11On12Device();
	CreateD2DComponents();

	m_frameIndex = 0;

	// Query the desktop's dpi settings, which will be used to create D2D's render targets.
	float dpi = static_cast<float>(GetDpiForWindow(m_hWnd));
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(m_backBufferFormat, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpi, dpi
	);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV, D2D render target, and a command allocator for each frame.
	for (UINT n = 0; n < FrameCount; n++)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_backBufferFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		GFX_THROW_INFO(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
		m_d3d12Device->CreateRenderTargetView(
			m_renderTargets[n].Get(), 
			&rtvDesc,
			rtvHeapHandle
		);
		NAME_D3D12_OBJECT_INDEXED(m_renderTargets, n);

		// Create a wrapped 11On12 resource of this back buffer. Since we are 
		// rendering all D3D12 content first and then all D2D content, we specify 
		// the In resource state as RENDER_TARGET - because D3D12 will have last 
		// used it in this state - and the Out resource state as PRESENT. When 
		// ReleaseWrappedResources() is called on the 11On12 device, the resource 
		// will be transitioned to the PRESENT state.
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		GFX_THROW_INFO(m_d3d11On12Device->CreateWrappedResource(
			m_renderTargets[n].Get(),
			&d3d11Flags,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT,
			IID_PPV_ARGS(&m_wrappedBackBuffers[n])
		));

		// Create a render target for D2D to draw directly to this back buffer.
		ComPtr<IDXGISurface> surface;
		GFX_THROW_INFO(m_wrappedBackBuffers[n].As(&surface));
		GFX_THROW_INFO(m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
			surface.Get(),
			&bitmapProperties,
			&m_d2dRenderTargets[n]
		));

		rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = static_cast<UINT64>(width);
	depthStencilDesc.Height = static_cast<UINT64>(height);
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	// depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.Format = m_depthStencilFormat;

	depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	D3D12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	GFX_THROW_INFO(
		m_d3d12Device->CreateCommittedResource(
			&props,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(m_depthStencilBuffer.ReleaseAndGetAddressOf())
		)
	);

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = m_depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_d3d12Device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_commandList->ResourceBarrier(1, &barrier);

	// Execute the resize commands.
	GFX_THROW_INFO(m_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();
}



void DeviceResourcesDX12::TestDraw(D3D12_VIEWPORT vp)
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	GFX_THROW_INFO(m_commandAllocator->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	GFX_THROW_INFO(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	// Indicate a state transition on the resource usage.
	D3D12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrier1);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	m_commandList->RSSetViewports(1, &vp);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DepthStencilView();

	// Clear the back buffer and depth buffer.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	CD3DX12_CPU_DESCRIPTOR_HANDLE _rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	m_commandList->ClearRenderTargetView(_rtvHandle, clearColor, 0, nullptr);
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.

	m_commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);





	// NOTE: do NOT transition the render target to present here.
	// the transition will occur when the wrapped 11On12 render
	// target resource is released.
	// 
	// Indicate a state transition on the resource usage.
	// Don't use this ->	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
	// Don't use this ->		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	// Don't use this ->	m_commandList->ResourceBarrier(1, &barrier2);

	// Done recording commands.
	GFX_THROW_INFO(m_commandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

D3D12_CPU_DESCRIPTOR_HANDLE DeviceResourcesDX12::DepthStencilView() const
{
	return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}
D3D12_CPU_DESCRIPTOR_HANDLE DeviceResourcesDX12::CurrentBackBufferView()const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_frameIndex,
		m_rtvDescriptorSize
	);
}
ID3D12Resource* DeviceResourcesDX12::CurrentBackBuffer()const
{
	return m_renderTargets[m_frameIndex].Get();
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
void DeviceResourcesDX12::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
{
	*ppAdapter = nullptr;

	ComPtr<IDXGIAdapter1> adapter;

	ComPtr<IDXGIFactory6> factory6;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
	{
		for (
			UINT adapterIndex = 0;
			SUCCEEDED(factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&adapter)));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	if (adapter.Get() == nullptr)
	{
		for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*ppAdapter = adapter.Detach();
}
void DeviceResourcesDX12::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	m_fenceValue++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	GFX_THROW_INFO(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));

	// Wait until the GPU has completed commands up to this fence point.
	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		GFX_THROW_INFO(m_fence->SetEventOnCompletion(m_fenceValue, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE); // TODO: Document these functions - Add check for eventHandle == 0?
		CloseHandle(eventHandle);
	}
}

void DeviceResourcesDX12::EnableGPUTimeout(bool enabled)
{
	// TODO: set m_enableGPUTimeout and recreate the device
}

void DeviceResourcesDX12::ClearBackground(const D2D1_COLOR_F& color)
{
	// TODO: Remove this function - make sure this still works when using D3D11

//	GFX_THROW_INFO_ONLY(
//		m_d3dDeviceContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0)
//	)
//
//	GFX_THROW_INFO_ONLY(
//		m_d3dDeviceContext->ClearRenderTargetView(m_d3dRenderTargetView.Get(), (float*)&color)
//	)
}

void DeviceResourcesDX12::Present()
{
	// swap the back and front buffers
	GFX_THROW_INFO(m_swapChain->Present(1, 0));
	m_frameIndex = (m_frameIndex + 1) % FrameCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();

//	DXGI_PRESENT_PARAMETERS parameters = { 0 };
//	HRESULT hRESULT = m_swapChain->Present1(1, 0, &parameters);
//
////	GFX_THROW_INFO_ONLY(
////		m_d3dDeviceContext->DiscardView1(m_d3dRenderTargetView.Get(), nullptr, 0)
////	)
////	GFX_THROW_INFO_ONLY(
////		m_d3dDeviceContext->DiscardView1(m_d3dDepthStencilView.Get(), nullptr, 0)
////	)
//
//	if (hRESULT == DXGI_ERROR_DEVICE_REMOVED || hRESULT == DXGI_ERROR_DEVICE_RESET)
//		HandleDeviceLost();
//	else // Throw if hr is a failed result and try to get debug info
//		GFX_THROW_INFO(hRESULT)
}

void DeviceResourcesDX12::BeginDraw()
{
	// Acquire our wrapped render target resource for the current back buffer.
	m_d3d11On12Device->AcquireWrappedResources(m_wrappedBackBuffers[m_frameIndex].GetAddressOf(), 1);

	// Render text directly to the back buffer.
	m_d2dDeviceContext->SetTarget(m_d2dRenderTargets[m_frameIndex].Get());
	m_d2dDeviceContext->BeginDraw();
}
void DeviceResourcesDX12::EndDraw()
{
	GFX_THROW_INFO(m_d2dDeviceContext->EndDraw());

	// Release our wrapped render target resource. Releasing 
	// transitions the back buffer resource to the state specified
	// as the OutState when the wrapped resource was created.
	m_d3d11On12Device->ReleaseWrappedResources(m_wrappedBackBuffers[m_frameIndex].GetAddressOf(), 1);

	// Flush to submit the 11 command list to the shared command queue.
	m_d3d11DeviceContext->Flush();

//	HRESULT hr = m_d2dDeviceContext->EndDraw();
//	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
//	{
//		HandleDeviceLost();
//	}
//
//	m_d2dDeviceContext->RestoreDrawingState(m_drawingStateBlock.Get());
}

void DeviceResourcesDX12::DrawLine(float x0, float y0, float x1, float y1, const D2D1_COLOR_F& color, float strokeWidth)
{
	ComPtr<ID2D1SolidColorBrush> brush;
	GFX_THROW_INFO(
		m_d2dDeviceContext->CreateSolidColorBrush(
			color,
			brush.ReleaseAndGetAddressOf()
		)
	)

	GFX_THROW_INFO_ONLY(
		m_d2dDeviceContext->DrawLine(
			{ x0, y0 },
			{ x1, y1 },
			brush.Get(), 
			strokeWidth)
	)
}


/*
void DeviceResourcesDX12::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	m_currentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	GFX_THROW_INFO(m_commandQueue->Signal(m_fence.Get(), m_currentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (m_fence->GetCompletedValue() < m_currentFence)
	{
		HANDLE eventHandle = CreateEventExW(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		GFX_THROW_INFO(m_fence->SetEventOnCompletion(m_currentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
*/

//D3D12_CPU_DESCRIPTOR_HANDLE DeviceResourcesDX12::DepthStencilView() const
//{
//	return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
//}


}

#endif // EG_DX11