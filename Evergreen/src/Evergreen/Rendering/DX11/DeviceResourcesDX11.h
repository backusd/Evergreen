#pragma once
#ifdef EG_DX11

#include "pch.h"
#include "DxgiInfoManagerDX11.h"
#include "Evergreen/Window/Window.h"
#include "Evergreen/Core.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API DeviceResourcesDX11
{
private:
	using _IDWriteFactory = IDWriteFactory7;
	using _IWICImagingFactory = IWICImagingFactory2;
	using _ID2D1Factory = ID2D1Factory7;
	using _ID2D1Device = ID2D1Device6;
	using _ID2D1DeviceContext = ID2D1DeviceContext6;
	using _ID2D1Bitmap = ID2D1Bitmap1;
	using _ID2D1DrawingStateBlock = ID2D1DrawingStateBlock1;

	using _ID3D11Device = ID3D11Device5;
	using _ID3D11DeviceContext = ID3D11DeviceContext4;
	using _ID3D11DepthStencilView = ID3D11DepthStencilView;
	using _ID3D11RenderTargetView = ID3D11RenderTargetView1;
	
	using _IDXGISwapChain = IDXGISwapChain4;
	using _IDXGIDevice = IDXGIDevice4;
	using _IDXGIFactory = IDXGIFactory5;


public:
	DeviceResourcesDX11(Window* window);
	DeviceResourcesDX11(const DeviceResourcesDX11&) = delete;
	void operator=(const DeviceResourcesDX11&) = delete;
	

	void OnResize(float width, float height);

	void BeginDraw() noexcept;
	void EndDraw();
	void Present();

	ND inline _IDWriteFactory*		DWriteFactory() const { return m_dwriteFactory.Get(); }
	ND inline _IWICImagingFactory*	WICImagingFactory() const { return m_wicImagingFactory.Get(); }
	ND inline _ID2D1Factory*		D2DFactory() const { return m_d2dFactory.Get(); }
	ND inline _ID2D1Device*			D2DDevice() const { return m_d2dDevice.Get(); }
	ND inline _ID2D1DeviceContext*	D2DDeviceContext() const { return m_d2dDeviceContext.Get(); }
	ND inline _ID2D1Bitmap*			D2DBitmap() const { return m_d2dBitmap.Get(); }

	ND inline _ID3D11Device* D3DDevice() noexcept { return m_d3dDevice.Get(); }
	ND inline _ID3D11DeviceContext* D3DDeviceContext() noexcept { return m_d3dDeviceContext.Get(); }
	ND inline _ID3D11DepthStencilView* DepthStencilView() noexcept { return m_d3dDepthStencilView.Get(); }
	ND inline _ID3D11RenderTargetView* BackBufferRenderTargetView() noexcept { return m_d3dRenderTargetView.Get(); }

	ND inline float DIPSToPixels(float dips) const noexcept { return dips * m_dpiScale; }
	ND inline float PixelsToDIPS(float pixels) const noexcept { return pixels / m_dpiScale; }

#if defined(_DEBUG)
public:
	ND inline static DxgiInfoManagerDX11& GetInfoManager() noexcept { return m_infoManager; }
private:
	static DxgiInfoManagerDX11 m_infoManager;
#endif

	
private:
	void CreateDeviceDependentResources();
	void CreateDeviceIndependentResources();
	void CreateWindowSizeDependentResources();
	void CreateWindowSizeDependentResources(float width, float height);
	void SetRenderTarget();
	void HandleDeviceLost();


	// Keep handle to window
	HWND m_hWnd;

	// DPI scale for the window
	float m_dpiScale;

	// Enable 2 sec GPU timeout
	bool m_gpuTimeoutEnabled = true;

	// ==========================================================
	// Direct 2D drawing components
	Microsoft::WRL::ComPtr<_ID2D1Factory>		m_d2dFactory;
	Microsoft::WRL::ComPtr<_ID2D1Device>		m_d2dDevice;
	Microsoft::WRL::ComPtr<_ID2D1DeviceContext>	m_d2dDeviceContext;
	Microsoft::WRL::ComPtr<_ID2D1Bitmap>		m_d2dBitmap;

	Microsoft::WRL::ComPtr<_ID2D1DrawingStateBlock> m_drawingStateBlock;

	// Direct Write drawing components
	Microsoft::WRL::ComPtr<_IDWriteFactory>		m_dwriteFactory;
	Microsoft::WRL::ComPtr<_IWICImagingFactory>	m_wicImagingFactory;

	// Direct3D objects
	Microsoft::WRL::ComPtr<_ID3D11Device>		 m_d3dDevice;
	Microsoft::WRL::ComPtr<_ID3D11DeviceContext> m_d3dDeviceContext;
	Microsoft::WRL::ComPtr<_IDXGISwapChain>		 m_dxgiSwapChain;

	// Direct3D Rendering objects
	Microsoft::WRL::ComPtr<_ID3D11RenderTargetView>	m_d3dRenderTargetView;
	Microsoft::WRL::ComPtr<_ID3D11DepthStencilView>	m_d3dDepthStencilView;

	// Cached device properties
	D3D_FEATURE_LEVEL m_d3dFeatureLevel;

	// Formats
	const DXGI_FORMAT m_rtvFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	const DXGI_FORMAT m_dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};
#pragma warning( pop )

}

#endif // EG_DX11