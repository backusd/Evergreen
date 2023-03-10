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
#if defined(_DEBUG) || defined(DBG)
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
	m_fenceValue(0),
	m_frameIndex(0)
/*	m_d2dFactory(nullptr),
	m_d2dDevice(nullptr),
	m_d2dDeviceContext(nullptr),
	m_d2dBitmap(nullptr),
	m_drawingStateBlock(nullptr),
	m_dwriteFactory(nullptr),
	m_wicImagingFactory(nullptr),
	m_dpiScale(96.0f),
	// DX12
	m_4xMsaaState(false),
	m_4xMsaaQuality(0),
	m_dxgiFactory(nullptr),
	m_swapChain(nullptr),
	m_d3dDevice(nullptr),
	m_fence(nullptr),
	m_currentFence(0),
	m_commandQueue(nullptr),
	m_directCmdListAlloc(nullptr),
	m_commandList(nullptr),
	m_currBackBuffer(0),
	m_depthStencilBuffer(nullptr),
	m_rtvHeap(nullptr),
	m_dsvHeap(nullptr),
	m_rtvDescriptorSize(0),
	m_dsvDescriptorSize(0),
	m_cbvSrvUavDescriptorSize(0),
	m_d3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	m_backBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
	m_depthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT)
	*/
{
	m_hWnd = window->GetHWND();
	m_aspectRatio = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
	m_scissorRect = CD3DX12_RECT(0, 0, static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

	// Must initialize COM library
	GFX_THROW_INFO(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))

	//LoadPipeline(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));
	//LoadAssets();

	CreateDXGIFactoryAndD3D12Device();
	CheckMultiSamplingSupport();
	CreateCommandObjects();
	CreateSwapChain(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));
	CreateD3D11On12Device();
	CreateD2DComponents();
	CreateRtvAndDsvDescriptorHeaps();
	CreateFence();

	OnResize(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

	/*
	CreateDeviceIndependentResources();
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

	m_d2dFactory->CreateDrawingStateBlock(m_drawingStateBlock.ReleaseAndGetAddressOf());

	// For now, assume the render target is NOT getting set by the Application class. 
	// Just set it here once for the lifetime of the application
	SetRenderTarget();
	*/
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
			// m_d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUGGABLE; TODO: Test this
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
		};

		D3D12_INFO_QUEUE_FILTER filter = {};
		//filter.DenyList.NumCategories = _countof(categories);
		//filter.DenyList.pCategoryList = categories;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;

		GFX_THROW_INFO(infoQueue->PushStorageFilter(&filter));
	}
#endif
}
void DeviceResourcesDX12::CheckMultiSamplingSupport()
{
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	GFX_THROW_INFO(
		m_d3d12Device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)
		)
	);

	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	EG_CORE_ASSERT(m_4xMsaaQuality > 0, "Unexpected MSAA quality level.");
}
void DeviceResourcesDX12::CreateCommandObjects()
{
	// ======================================================================
	// Command Queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

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
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = m_backBufferFormat;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	
	// TODO: Look into these ???
	//swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	//swapChainDesc.Flags = 0;
	//swapChainDesc.Scaling = DXGI_SCALING_NONE;
	//swapChainDesc.Stereo = false;

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
	GFX_THROW_INFO(m_dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

	GFX_THROW_INFO(swapChain.As(&m_swapChain));

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}
void DeviceResourcesDX12::CreateD3D11On12Device()
{
	m_d3d11DeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

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
	D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	GFX_THROW_INFO(
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, 
			__uuidof(ID2D1Factory3), 
			&m_d2dFactoryOptions, 
			&m_d2dFactory
		)
	);
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
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	GFX_THROW_INFO(m_d3d12Device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.ReleaseAndGetAddressOf())
	));

	// Depth Stencil View
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	GFX_THROW_INFO(m_d3d12Device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.ReleaseAndGetAddressOf())
	));





/*
	// Query the desktop's dpi settings, which will be used to create D2D's render targets.
	float dpi = static_cast<float>(GetDpiForWindow(m_hWnd));
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpi,
		dpi
	);

	m_rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// ===============================================================================
	// Render Target View
	// 
	// Describe and create a render target view (RTV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	GFX_THROW_INFO(m_d3d12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.ReleaseAndGetAddressOf())));

	// Create frame resources.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV, D2D render target, and a command allocator for each frame.
	for (UINT n = 0; n < FrameCount; n++)
	{
		GFX_THROW_INFO(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
		m_d3d12Device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);

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

		rtvHandle.Offset(1, m_rtvDescriptorSize);

		
		//GFX_THROW_INFO(
		//	m_d3d12Device->CreateCommandAllocator(
		//		D3D12_COMMAND_LIST_TYPE_DIRECT, 
		//		IID_PPV_ARGS(&m_commandAllocators[n])
		//	)
		//);
	}


	// ===============================================================================
	// Depth Stencil
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	GFX_THROW_INFO(m_d3d12Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.ReleaseAndGetAddressOf())));






	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	GFX_THROW_INFO(
		m_d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
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

*/
}
void DeviceResourcesDX12::CreateFence()
{
	GFX_THROW_INFO(
		m_d3d12Device->CreateFence(
			m_fenceValue, 
			D3D12_FENCE_FLAG_NONE, 
			IID_PPV_ARGS(m_fence.GetAddressOf())
		)
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE DeviceResourcesDX12::DepthStencilView() const
{
	return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void DeviceResourcesDX12::OnResize(float width, float height)
{
	EG_CORE_ASSERT(m_d3d12Device != nullptr, "No D3D12 device");
	EG_CORE_ASSERT(m_swapChain != nullptr, "No swap chain");
	EG_CORE_ASSERT(m_commandList != nullptr, "No command list");

	// Flush before changing any resources.
	FlushCommandQueue();

	GFX_THROW_INFO(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < FrameCount; ++i)
		m_renderTargets[i].Reset();
	m_depthStencilBuffer.Reset();

	// Resize the swap chain.
	GFX_THROW_INFO(
		m_swapChain->ResizeBuffers(
			FrameCount,
			width,
			height,
			m_backBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		)
	);

	m_frameIndex = 0;

	// Query the desktop's dpi settings, which will be used to create D2D's render targets.
	float dpi = static_cast<float>(GetDpiForWindow(m_hWnd));
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpi,
		dpi
	);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV, D2D render target, and a command allocator for each frame.
	for (UINT n = 0; n < FrameCount; n++)
	{
		GFX_THROW_INFO(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
		m_d3d12Device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHeapHandle);
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
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
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
	//D3D12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
	//	D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//m_commandList->ResourceBarrier(1, &barrier1);
	
	D3D12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrier1);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	m_commandList->RSSetViewports(1, &vp);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CurrentBackBufferView();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DepthStencilView();

	// Clear the back buffer and depth buffer.
	const float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.

	//m_commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Note: do not transition the render target to present here.
	// the transition will occur when the wrapped 11On12 render
	// target resource is released.
	// 
	// Indicate a state transition on the resource usage.
	//	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
	//		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	//	m_commandList->ResourceBarrier(1, &barrier2);

	// Done recording commands.
	GFX_THROW_INFO(m_commandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);






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












/*
void DeviceResourcesDX12::LoadPipeline(float width, float height)
{
	UINT dxgiFactoryFlags = 0;
	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

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
			d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
			d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
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

//	if (m_useWarpDevice)
//	{
//		ComPtr<IDXGIAdapter> warpAdapter;
//		GFX_THROW_INFO(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
//
//		GFX_THROW_INFO(D3D12CreateDevice(
//			warpAdapter.Get(),
//			D3D_FEATURE_LEVEL_11_0,
//			IID_PPV_ARGS(&m_d3d12Device)
//		));
//	}
//	else
//	{
//		// NOTE: D3D12CreateDevice() will use the 1st hardware adapter it can find if nullptr is passed
//		//       as the first parameter. This is fine, if the 1st adapter happens to support D3D12, but
//		//       it is better to instead search for a D3D12 compatible adapter
//		ComPtr<IDXGIAdapter1> hardwareAdapter;
//		GetHardwareAdapter(m_dxgiFactory.Get(), &hardwareAdapter);
//
//		GFX_THROW_INFO(D3D12CreateDevice(
//			hardwareAdapter.Get(),
//			D3D_FEATURE_LEVEL_11_0,
//			IID_PPV_ARGS(&m_d3d12Device)
//		));
//	}

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
		};

		D3D12_INFO_QUEUE_FILTER filter = {};
		//filter.DenyList.NumCategories = _countof(categories);
		//filter.DenyList.pCategoryList = categories;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;

		GFX_THROW_INFO(infoQueue->PushStorageFilter(&filter));
	}
#endif

	m_rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	GFX_THROW_INFO(m_d3d12Device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	GFX_THROW_INFO(m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	NAME_D3D12_OBJECT(m_commandQueue);




	// Describe the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;
	GFX_THROW_INFO(m_dxgiFactory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
		m_hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	// This sample does not support fullscreen transitions.
	GFX_THROW_INFO(m_dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

	GFX_THROW_INFO(swapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create an 11 device wrapped around the 12 device and share
	// 12's command queue.
	ComPtr<ID3D11Device> d3d11Device;
	GFX_THROW_INFO(D3D11On12CreateDevice(
		m_d3d12Device.Get(),
		d3d11DeviceFlags,
		nullptr,
		0,
		reinterpret_cast<IUnknown**>(m_commandQueue.GetAddressOf()),
		1,
		0,
		&d3d11Device,
		&m_d3d11DeviceContext,
		nullptr
	));

	// Query the 11On12 device from the 11 device.
	GFX_THROW_INFO(d3d11Device.As(&m_d3d11On12Device));

	// Create D2D/DWrite components.
	{
		D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		GFX_THROW_INFO(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &m_d2dFactory));
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

	// Query the desktop's dpi settings, which will be used to create
	// D2D's render targets.
	float dpiX;
	float dpiY;
#pragma warning(push)
#pragma warning(disable : 4996) // GetDesktopDpi is deprecated.
	m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX,
		dpiY
	);

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		GFX_THROW_INFO(m_d3d12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		m_rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV, D2D render target, and a command allocator for each frame.
		for (UINT n = 0; n < FrameCount; n++)
		{
			GFX_THROW_INFO(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_d3d12Device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);

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

			rtvHandle.Offset(1, m_rtvDescriptorSize);

			GFX_THROW_INFO(m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n])));
		}
	}
}
void DeviceResourcesDX12::LoadAssets()
{
//	// Create an empty root signature.
//	{
//		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
//		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
//
//		ComPtr<ID3DBlob> signature;
//		ComPtr<ID3DBlob> error;
//		GFX_THROW_INFO(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
//		GFX_THROW_INFO(m_d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
//		NAME_D3D12_OBJECT(m_rootSignature);
//	}
//
//	// Create the pipeline state, which includes compiling and loading shaders.
//	{
//		ComPtr<ID3DBlob> vertexShader;
//		ComPtr<ID3DBlob> pixelShader;
//
//#if defined(_DEBUG)
//		// Enable better shader debugging with the graphics debugging tools.
//		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//#else
//		UINT compileFlags = 0;
//#endif
//
//		GFX_THROW_INFO(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
//		GFX_THROW_INFO(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
//
//		// Define the vertex input layout.
//		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
//		{
//			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
//		};
//
//		// Describe and create the graphics pipeline state object (PSO).
//		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
//		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
//		psoDesc.pRootSignature = m_rootSignature.Get();
//		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
//		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
//		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//		psoDesc.DepthStencilState.DepthEnable = FALSE;
//		psoDesc.DepthStencilState.StencilEnable = FALSE;
//		psoDesc.SampleMask = UINT_MAX;
//		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//		psoDesc.NumRenderTargets = 1;
//		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
//		psoDesc.SampleDesc.Count = 1;
//
//		GFX_THROW_INFO(m_d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
//		NAME_D3D12_OBJECT(m_pipelineState);
//	}

//	GFX_THROW_INFO(
//		m_d3d12Device->CreateCommandList(
//			0, 
//			D3D12_COMMAND_LIST_TYPE_DIRECT, 
//			m_commandAllocators[m_frameIndex].Get(), 
//			m_pipelineState.Get(), 
//			IID_PPV_ARGS(&m_commandList))
//	);
	GFX_THROW_INFO(
		m_d3d12Device->CreateCommandList(
			0,											// NodeMask: "For single-GPU operation, set this to zero."
			D3D12_COMMAND_LIST_TYPE_DIRECT,				// Type: Direct = "Specifies a command buffer that the GPU can execute. A direct command list doesn't inherit any GPU state."
			m_commandAllocators[m_frameIndex].Get(),	// Command Allocator: "A pointer to the command allocator object from which the device creates command lists."
			nullptr,									// PipelineState: "If it is nullptr, then the runtime sets a dummy initial pipeline state, so that drivers don't have to deal with undefined state."
			IID_PPV_ARGS(&m_commandList))
	);
	NAME_D3D12_OBJECT(m_commandList);

//	// Create D2D/DWrite objects for rendering text.
//	{
//		GFX_THROW_INFO(m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_textBrush));
//		GFX_THROW_INFO(m_dWriteFactory->CreateTextFormat(
//			L"Verdana",
//			NULL,
//			DWRITE_FONT_WEIGHT_NORMAL,
//			DWRITE_FONT_STYLE_NORMAL,
//			DWRITE_FONT_STRETCH_NORMAL,
//			50,
//			L"en-us",
//			&m_textFormat
//		));
//		GFX_THROW_INFO(m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
//		GFX_THROW_INFO(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
//	}

//	// Note: ComPtr's are CPU objects but this resource needs to stay in scope until
//	// the command list that references it has finished executing on the GPU.
//	// We will flush the GPU at the end of this method to ensure the resource is not
//	// prematurely destroyed.
//	ComPtr<ID3D12Resource> vertexBufferUpload;
//
//	// Create the vertex buffer.
//	{
//		// Define the geometry for a triangle.
//		Vertex triangleVertices[] =
//		{
//			{ { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
//			{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
//			{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
//		};
//
//		const UINT vertexBufferSize = sizeof(triangleVertices);
//
//		GFX_THROW_INFO(m_d3d12Device->CreateCommittedResource(
//			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
//			D3D12_HEAP_FLAG_NONE,
//			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
//			D3D12_RESOURCE_STATE_COPY_DEST,
//			nullptr,
//			IID_PPV_ARGS(&m_vertexBuffer)));
//
//		GFX_THROW_INFO(m_d3d12Device->CreateCommittedResource(
//			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//			D3D12_HEAP_FLAG_NONE,
//			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
//			D3D12_RESOURCE_STATE_GENERIC_READ,
//			nullptr,
//			IID_PPV_ARGS(&vertexBufferUpload)));
//
//		NAME_D3D12_OBJECT(m_vertexBuffer);
//
//		// Copy data to the intermediate upload heap and then schedule a copy 
//		// from the upload heap to the vertex buffer.
//		D3D12_SUBRESOURCE_DATA vertexData = {};
//		vertexData.pData = reinterpret_cast<UINT8*>(triangleVertices);
//		vertexData.RowPitch = vertexBufferSize;
//		vertexData.SlicePitch = vertexData.RowPitch;
//
//		UpdateSubresources<1>(m_commandList.Get(), m_vertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);
//		m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
//
//		// Initialize the vertex buffer view.
//		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
//		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
//		m_vertexBufferView.SizeInBytes = vertexBufferSize;
//	}

	// Close the command list and execute it to begin the vertex buffer copy into
	// the default heap.
	GFX_THROW_INFO(m_commandList->Close());
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		GFX_THROW_INFO(m_d3d12Device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
		m_fenceValues[m_frameIndex]++;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			GFX_THROW_INFO(HRESULT_FROM_WIN32(GetLastError()));
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForGpu();
	}
}
*/
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
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

/*
void DeviceResourcesDX12::WaitForGpu()
{
	// Schedule a Signal command in the queue.
	GFX_THROW_INFO(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

	// Wait until the fence has been processed.
	GFX_THROW_INFO(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	m_fenceValues[m_frameIndex]++;
}
*/

void DeviceResourcesDX12::EnableGPUTimeout(bool enabled)
{
	// TODO: set m_enableGPUTimeout and recreate the device
}

/*
void DeviceResourcesDX12::CreateDeviceIndependentResources()
{
	// Initialize Direct2D Resources
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// If the project is in debug, enable Direct2D debugging
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	GFX_THROW_INFO(
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory7),
			&options,
			(void**)m_d2dFactory.ReleaseAndGetAddressOf()
		)
	);

	GFX_THROW_INFO(
		DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory7),
			reinterpret_cast<IUnknown**>(m_dwriteFactory.ReleaseAndGetAddressOf())
		)
	);

	GFX_THROW_INFO(
		CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(m_wicImagingFactory.ReleaseAndGetAddressOf())
		)
	);
}
*/
/*
void DeviceResourcesDX12::CreateDeviceDependentResources()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		GFX_THROW_INFO(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (SdkLayersAvailable())
	{
		// If the project is in a debug build, enable debugging via SDK Layers with this flag
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory1> dxgiFactory;
	GFX_THROW_INFO(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
	GFX_THROW_INFO(dxgiFactory.As(&m_dxgiFactory));

	// Try to create hardware device.
	ComPtr<ID3D12Device> device;
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device)
	);

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		GFX_THROW_INFO(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		device = nullptr;
		GFX_THROW_INFO(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&device))
		);
	}
	
	GFX_THROW_INFO(device.As(&m_d3dDevice));

	GFX_THROW_INFO(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	GFX_THROW_INFO(m_d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels))
	);

	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	EG_CORE_ASSERT(m_4xMsaaQuality > 0, "Unexpected MSAA quality level.");


//	// This array defines the set of DirectX hardware feature levels this app will support
//	// Note: the ordering should be preserved
//	// Don't forget to declare your application's minimum required feature level in its
//	// description. All applications are assumed to support 9.1 unless otherwise stated
//	D3D_FEATURE_LEVEL featureLevels[] =
//	{
//		D3D_FEATURE_LEVEL_12_1,
//		D3D_FEATURE_LEVEL_12_0,
//		D3D_FEATURE_LEVEL_11_1,
//		D3D_FEATURE_LEVEL_11_0,
//		D3D_FEATURE_LEVEL_10_1,
//		D3D_FEATURE_LEVEL_10_0,
//		D3D_FEATURE_LEVEL_9_3,
//		D3D_FEATURE_LEVEL_9_2,
//		D3D_FEATURE_LEVEL_9_1
//	};
//
//	ComPtr<ID3D11Device> device;
//	ComPtr<ID3D11DeviceContext> context;
//	
//	// Throw a DeviceResourceException if creating the device fails
//	GFX_THROW_INFO(D3D11CreateDevice(
//		nullptr,					// nullptr to use default adapter
//		D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver
//		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE
//		creationFlags,				// Set debug and Direct2D compatibility flags
//		featureLevels,				// List of feature levels this app can support
//		ARRAYSIZE(featureLevels),	// Size of feature levels list
//		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Windows Store apps
//		device.ReleaseAndGetAddressOf(),		// Address of pointer to Direct3D device being created
//		&m_d3dFeatureLevel,			// Address of feature level of device that was created
//		context.ReleaseAndGetAddressOf()		// Address of pointer to device context being created
//	));
//
//	// Don't try on a WARP device as I'm guessing it will be too slow for our purposes
//
//	if (FAILED(hr))
//	{
//		// If the initialization fails, fall back to the WARP device
//		// For more information on WARP, see:
//		// http://go.microsoft.com/fwlink/?LinkId=286690
//		GFX_THROW_INFO(
//			D3D11CreateDevice(
//				nullptr,
//				D3D_DRIVER_TYPE_WARP,		// Create a WARP device instead of hardware device
//				0,
//				creationFlags,
//				featureLevels,
//				ARRAYSIZE(featureLevels),
//				D3D11_SDK_VERSION,
//				device.ReleaseAndGetAddressOf(),
//				&m_d3dFeatureLevel,
//				context.ReleaseAndGetAddressOf()
//			)
//		);
//	}
//
//	// Store pointers to the Direct3D 11.3 API device and immediate context
//	GFX_THROW_INFO(device.As(&m_d3dDevice));
//
//	GFX_THROW_INFO(context.As(&m_d3dDeviceContext));

	// Create the Direct2D device object and a corresponding context


//	ComPtr<IDXGIDevice> dxgiDevice;
//	GFX_THROW_INFO(m_d3dDevice.As(&dxgiDevice));
//
//	GFX_THROW_INFO(
//		m_d2dFactory->CreateDevice(
//			dxgiDevice.Get(),
//			m_d2dDevice.ReleaseAndGetAddressOf()
//		)
//	);
//
//	GFX_THROW_INFO(
//		m_d2dDevice->CreateDeviceContext(
//			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
//			m_d2dDeviceContext.ReleaseAndGetAddressOf()
//		)
//	);

	// ====================================================================
	// CreateCommandObjects()
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	GFX_THROW_INFO(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	GFX_THROW_INFO(m_d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_directCmdListAlloc.GetAddressOf()))
	);

	GFX_THROW_INFO(m_d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_directCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(m_commandList.GetAddressOf()))
	);

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	m_commandList->Close();

	// ====================================================================
	// CreateSwapChain()
	
	// Get height, width, and dpi for the window
	RECT rect;
	if (GetClientRect(m_hWnd, &rect) == 0)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	float height = static_cast<float>(rect.bottom - rect.top);
	float width = static_cast<float>(rect.right - rect.left);
	
	// Release the previous swapchain we will be recreating.
	m_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_backBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = m_hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ComPtr<IDXGISwapChain> swapChain;
	GFX_THROW_INFO(m_dxgiFactory->CreateSwapChain(
		m_commandQueue.Get(),
		&sd,
		swapChain.GetAddressOf())
	);
	GFX_THROW_INFO(swapChain.As(&m_swapChain));


	// ====================================================================
	// CreateRtvAndDsvDescriptorHeaps()
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	GFX_THROW_INFO(
		m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf()))
	);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	GFX_THROW_INFO(
		m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf()))
	);
}
*/
/*
void DeviceResourcesDX12::CreateWindowSizeDependentResources()
{
	// Get height, width, and dpi for the window
	RECT rect;
	if (GetClientRect(m_hWnd, &rect) == 0)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	float height = static_cast<float>(rect.bottom - rect.top);
	float width = static_cast<float>(rect.right - rect.left);

	CreateWindowSizeDependentResources(width, height);
}
*/
/*
void DeviceResourcesDX12::CreateWindowSizeDependentResources(float width, float height)
{
	float dpi = static_cast<float>(GetDpiForWindow(m_hWnd));
	m_dpiScale = dpi / 96.0f;

	// Clear the previous window size specific context
//	m_d3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr); // These are the appropriate parameters to clear the render targets
//	m_d3dRenderTargetView = nullptr;
//	m_d2dDeviceContext->SetTarget(nullptr);
//	m_d2dBitmap = nullptr;
//	m_d3dDepthStencilView = nullptr;
//	m_d3dDeviceContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	// if the swap chain already exists, resize it
	if (m_swapChain.Get() != nullptr)
	{
		HRESULT hRESULT = m_swapChain->ResizeBuffers(
			SwapChainBufferCount,
			lround(width),
			lround(height),
			m_backBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		);

		if (hRESULT == DXGI_ERROR_DEVICE_REMOVED || hRESULT == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created
			HandleDeviceLost();

			// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method
			// and correctly set up the new device
			return;
		}
		else
		{
			GFX_THROW_INFO(hRESULT);
		}
	}
	else
	{

//		//DXGI_SCALING scaling = DO I Support High Resolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
//		DXGI_SCALING scaling = DXGI_SCALING_NONE;
//
//		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
//		swapChainDesc.Width = static_cast<UINT>(width);					// Match the size of the window
//		swapChainDesc.Height = static_cast<UINT>(height);
//		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;				// This is the most common swap chain format
//		swapChainDesc.Stereo = false;
//		swapChainDesc.SampleDesc.Count = 1;								// Don't use multi-sampling
//		swapChainDesc.SampleDesc.Quality = 0;
//		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		swapChainDesc.BufferCount = 2;									// Use double-buffering to minimize latency
//		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Windows Store apps must use this SwapEffect
//		swapChainDesc.Flags = 0;
//		swapChainDesc.Scaling = scaling;
//		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
//
//		// This sequence obtains the DXGI factory that was used to create the Direct3D device above
//		ComPtr<IDXGIDevice4> dxgiDevice;
//		GFX_THROW_INFO(m_d3dDevice.As(&dxgiDevice));
//
//		ComPtr<IDXGIAdapter> dxgiAdapter;
//		GFX_THROW_INFO(
//			dxgiDevice->GetAdapter(dxgiAdapter.ReleaseAndGetAddressOf())
//		);
//
//		ComPtr<IDXGIFactory5> dxgiFactory;
//		GFX_THROW_INFO(
//			dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()))
//		);
//
//
//		// THIS NEEDS VERIFICATION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		ComPtr<IDXGISwapChain1> swapChain;
//		GFX_THROW_INFO(
//			dxgiFactory->CreateSwapChainForHwnd(
//				m_d3dDevice.Get(),
//				m_hWnd,
//				&swapChainDesc,
//				nullptr,
//				nullptr,
//				swapChain.ReleaseAndGetAddressOf()
//			)
//		);
//		GFX_THROW_INFO(swapChain.As(&m_dxgiSwapChain));

		// -----------------------------------------------------------------------------------------------
		m_swapChain.Reset();
		DXGI_SCALING scaling = DXGI_SCALING_NONE;

		DXGI_SWAP_CHAIN_DESC1 sd;
		sd.Width = static_cast<UINT>(width);
		sd.Height = static_cast<UINT>(height);
		sd.Format = m_backBufferFormat;				
		sd.Stereo = false;
		sd.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
		sd.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = SwapChainBufferCount;	
		// TODO: Look into the SwapEffect options (and test them?)
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Windows Store apps must use this SwapEffect
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.Scaling = DXGI_SCALING_NONE; // TODO: Look into this
		sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// TODO: THIS NEEDS VERIFICATION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		ComPtr<IDXGISwapChain1> swapChain;
		GFX_THROW_INFO(
			m_dxgiFactory->CreateSwapChainForHwnd(
				m_commandQueue.Get(),
				m_hWnd,
				&sd,
				nullptr,
				nullptr,
				swapChain.ReleaseAndGetAddressOf()
			)
		);
		GFX_THROW_INFO(swapChain.As(&m_swapChain));


		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each Vsync, minimizing power consumption
		// TODO: See if there are any other functions we should call on dxgiDevice
		ComPtr<IDXGIDevice4> dxgiDevice;
		GFX_THROW_INFO(m_d3dDevice.As(&dxgiDevice));
		GFX_THROW_INFO(dxgiDevice->SetMaximumFrameLatency(1));
	}

	GFX_THROW_INFO(
		m_swapChain->SetRotation(DXGI_MODE_ROTATION_IDENTITY)
	);

	// ======================================================================================================
	// Flush before changing any resources.
	FlushCommandQueue();

	GFX_THROW_INFO(m_commandList->Reset(m_directCmdListAlloc.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i)
		m_swapChainBuffer[i].Reset();
	m_depthStencilBuffer.Reset();

	// Resize the swap chain.
	GFX_THROW_INFO(m_swapChain->ResizeBuffers(
		SwapChainBufferCount,
		width, height,
		m_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	m_currBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		GFX_THROW_INFO(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i])));
		m_d3dDevice->CreateRenderTargetView(m_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

//	GFX_THROW_INFO(
//		m_d3dDevice->CreateCommittedResource(
//			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
//			D3D12_HEAP_FLAG_NONE,
//			&depthStencilDesc,
//			D3D12_RESOURCE_STATE_COMMON,
//			&optClear,
//			IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())
//		)
//	);

	D3D12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	m_d3dDevice->CreateCommittedResource(
		&props,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())
	);

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = m_depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

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

//	// Create a render target view of the swap chain back buffer
//	ComPtr<ID3D11Texture2D1> backBuffer;
//	GFX_THROW_INFO(
//		m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ReleaseAndGetAddressOf()))
//	);
//
//	GFX_THROW_INFO(
//		m_d3dDevice->CreateRenderTargetView1(
//			backBuffer.Get(),
//			nullptr,
//			m_d3dRenderTargetView.ReleaseAndGetAddressOf()
//		)
//	);
//
//	// Create a depth stencil view for use with 3D rendering if needed
//	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
//		DXGI_FORMAT_D24_UNORM_S8_UINT, // reserve 24 bits for the depth value and 8 bits for stencil value (used for outline effect)
//		static_cast<UINT>(width),
//		static_cast<UINT>(height),
//		1, // This depth stencil view has only one texture
//		1, // Use a single mipmap level
//		D3D11_BIND_DEPTH_STENCIL
//	);
//
//	ComPtr<ID3D11Texture2D1> depthStencil;
//	GFX_THROW_INFO(
//		m_d3dDevice->CreateTexture2D1(
//			&depthStencilDesc,
//			nullptr,
//			depthStencil.ReleaseAndGetAddressOf()
//		)
//	);
//
//	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(
//		D3D11_DSV_DIMENSION_TEXTURE2D,
//		DXGI_FORMAT_D24_UNORM_S8_UINT
//	);
//	GFX_THROW_INFO(
//		m_d3dDevice->CreateDepthStencilView(
//			depthStencil.Get(),
//			&depthStencilViewDesc,
//			m_d3dDepthStencilView.ReleaseAndGetAddressOf()
//		)
//	);

//	// Set the rasterizer state
//	D3D11_RASTERIZER_DESC rd;
//	rd.FillMode = D3D11_FILL_SOLID; // or D3D11_FILL_WIREFRAME
//	rd.CullMode = D3D11_CULL_NONE;
//	rd.FrontCounterClockwise = true;	// This must be true for the outline effect to work properly
//	rd.DepthBias = 0;
//	rd.SlopeScaledDepthBias = 0.0f;
//	rd.DepthBiasClamp = 0.0f;
//	rd.DepthClipEnable = true;
//	rd.ScissorEnable = false;
//	rd.MultisampleEnable = false;
//	rd.AntialiasedLineEnable = false;
//
//	// Create and set the solid raster state
//	GFX_THROW_INFO(
//		m_d3dDevice->CreateRasterizerState(&rd, solidRasterState.ReleaseAndGetAddressOf())
//	);
//	m_d3dDeviceContext->RSSetState(solidRasterState.Get());
//
//	// Also create the wireframe raster state
//	rd.FillMode = D3D11_FILL_WIREFRAME;
//	GFX_THROW_INFO(
//		m_d3dDevice->CreateRasterizerState(&rd, wireframeRasterState.ReleaseAndGetAddressOf())
//	);

	// Create a Direct2D target bitmap associated with the
	// swap cahin back buffer and set it as the current target
//	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
//		D2D1::BitmapProperties1(
//			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
//			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
//			dpi,
//			dpi
//		);
//
//	ComPtr<IDXGISurface2> dxgiBackBuffer;
//	GFX_THROW_INFO(
//		m_swapChain->GetBuffer(0, IID_PPV_ARGS(dxgiBackBuffer.ReleaseAndGetAddressOf()))
//	);
//
//	GFX_THROW_INFO(
//		m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
//			dxgiBackBuffer.Get(),
//			&bitmapProperties,
//			m_d2dBitmap.ReleaseAndGetAddressOf()
//		)
//	);
//
//	m_d2dDeviceContext->SetTarget(m_d2dBitmap.Get());
//
//	m_d2dDeviceContext->SetDpi(dpi, dpi);
//
//	// Grayscale text anti-aliasing is recommended for all Windows Store apps
//	m_d2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}
*/




// Recreate all device resources and set them back to the current state
void DeviceResourcesDX12::HandleDeviceLost()
{
//	m_swapChain = nullptr;
//	CreateDeviceDependentResources();
//	CreateWindowSizeDependentResources();
//
//	// Must reset the render target because it gets deleted/recreated when we call CreateWindowSizeDependentResources
//	SetRenderTarget();
}

//void DeviceResourcesDX12::SetRenderTarget()
//{
//	ID3D11RenderTargetView* const targets[1] = { m_d3dRenderTargetView.Get() };
//	GFX_THROW_INFO_ONLY(
//		m_d3dDeviceContext->OMSetRenderTargets(1, targets, m_d3dDepthStencilView.Get())
//	)
//}

void DeviceResourcesDX12::ClearBackground(const D2D1_COLOR_F& color)
{
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
	GFX_THROW_INFO(m_swapChain->Present(0, 0));
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