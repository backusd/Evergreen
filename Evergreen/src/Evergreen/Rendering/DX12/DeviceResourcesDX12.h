#pragma once
#ifdef EG_DX12

#include "pch.h"
#include "DxgiInfoManagerDX12.h"
#include "Evergreen/Window/Window.h"
#include "Evergreen/Core.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API DeviceResourcesDX12
{
public:
	DeviceResourcesDX12(Window* window);
	DeviceResourcesDX12(const DeviceResourcesDX12&) = delete;
	void operator=(const DeviceResourcesDX12&) = delete;

	void ClearBackground(const D2D1_COLOR_F& color);

	void Present();

	void OnResize(float width, float height);

	void DrawLine(float x0, float y0, float x1, float y1, const D2D1_COLOR_F& color, float strokeWidth = 1.0f);

	void BeginDraw();
	void EndDraw();

	void TestDraw(D3D12_VIEWPORT vp);

	ND inline IDWriteFactory*		DWriteFactory() const { return m_dWriteFactory.Get(); }
	ND inline IWICImagingFactory2*	WICImagingFactory() const { return m_wicImagingFactory.Get(); }
	ND inline ID2D1Factory3*		D2DFactory() const { return m_d2dFactory.Get(); }
	ND inline ID2D1Device2*			D2DDevice() const { return m_d2dDevice.Get(); }
	ND inline ID2D1DeviceContext2*	D2DDeviceContext() const { return m_d2dDeviceContext.Get(); }
//	ND inline ID2D1Bitmap1*			D2DBitmap() const { return m_d2dBitmap.Get(); }

	
	//static void SetViewport(D3D11_VIEWPORT viewport) noexcept { Get().SetViewportImpl(viewport); }

	//D3D11_VIEWPORT GetViewport() noexcept { return Get().m_viewport; }
//	ID3D11Device5* D3DDevice() noexcept { return m_d3dDevice.Get(); }
//	ID3D11DeviceContext4* D3DDeviceContext() noexcept { return m_d3dDeviceContext.Get(); }
//	ID3D11DepthStencilView* DepthStencilView() noexcept { return m_d3dDepthStencilView.Get(); }
//	ID3D11RenderTargetView1* BackBufferRenderTargetView() noexcept { return m_d3dRenderTargetView.Get(); }

	//void FlushCommandQueue();

	void EnableGPUTimeout(bool enabled);

#if defined(_DEBUG)
public:
	ND inline static DxgiInfoManagerDX12& GetInfoManager() noexcept { return m_infoManager; }
private:
	static DxgiInfoManagerDX12 m_infoManager;
#endif

	
private:
	// Keep handle to window
	HWND m_hWnd;

	void CreateDXGIFactoryAndD3D12Device();
	// void CheckMultiSamplingSupport(); <-- Can omit this for now. D3D12 does not support multisampling (see comment below)
	void CreateCommandObjects();
	void CreateSwapChain(float width, float height);
	void CreateD3D11On12Device();
	void CreateD2DComponents();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateFence();

	void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false);
	
	void FlushCommandQueue();

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	ID3D12Resource* CurrentBackBuffer() const;

	static const UINT FrameCount = 2;

	// Pipeline objects.
	CD3DX12_RECT										m_scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3>				m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_d3d11DeviceContext;
	Microsoft::WRL::ComPtr<ID3D11On12Device>			m_d3d11On12Device;
	Microsoft::WRL::ComPtr<ID3D12Device>				m_d3d12Device;
	Microsoft::WRL::ComPtr<IDWriteFactory>				m_dWriteFactory;
	Microsoft::WRL::ComPtr<ID2D1Factory3>				m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device2>				m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext2>			m_d2dDeviceContext;
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_renderTargets[FrameCount];
	Microsoft::WRL::ComPtr<ID3D11Resource>				m_wrappedBackBuffers[FrameCount];
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>				m_d2dRenderTargets[FrameCount];
	// Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_commandAllocators[FrameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>			m_commandQueue;
	// Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_commandList;

	Microsoft::WRL::ComPtr<IWICImagingFactory2>			m_wicImagingFactory;
	Microsoft::WRL::ComPtr<IDXGIFactory7>				m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_depthStencilBuffer;

	// App resources.
	UINT m_rtvDescriptorSize;
	UINT m_dsvDescriptorSize;
	UINT m_cbvSrvUavDescriptorSize;

	// Synchronization objects.
	UINT								m_frameIndex;
	HANDLE								m_fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64								m_fenceValue;

	DXGI_FORMAT m_backBufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// NOTE: Multisampling does NOT have native support in D3D12. There are methods to manually
	//       perform multisampling, but require a fair bit of extra work that I am not going to 
	//       do at this time.
	//       See: https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
	const bool m_4xMsaaState = false;
	const UINT m_4xMsaaQuality = 0;

	// This boolean controls whether the D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT flag is set on m_d3d11DeviceFlags
	//     "Use this flag if the device will produce GPU workloads that take more than two seconds to complete, 
	//     and you want the operating system to allow them to successfully finish. If this flag is not set, the 
	//     operating system performs timeout detection and recovery when it detects a GPU packet that took more 
	//     than two seconds to execute. If this flag is set, the operating system allows such a long running packet 
	//     to execute without resetting the GPU. We recommend not to set this flag if your device needs to be 
	//     highly responsive so that the operating system can detect and recover from GPU timeouts. We recommend to 
	//     set this flag if your device needs to perform time consuming background tasks such as compute, image 
	//     recognition, and video encoding to allow such tasks to successfully finish."
	bool m_enableGPUTimeout;

	float m_aspectRatio;

	UINT m_d3d11DeviceFlags;
	D2D1_FACTORY_OPTIONS m_d2dFactoryOptions;
};
#pragma warning( pop )

}

#endif // EG_DX11