#pragma once
#ifdef EG_DX11

#include "pch.h"
#include "DxgiInfoManagerDX11.h"
#include "Evergreen/UI/Color.h"
#include "Evergreen/Window/Window.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API DeviceResourcesDX11
{
public:
	DeviceResourcesDX11(Window* window) noexcept;
	DeviceResourcesDX11(const DeviceResourcesDX11&) = delete;
	void operator=(const DeviceResourcesDX11&) = delete;

	void ClearBackground(const Color& color) noexcept;

	void Present() noexcept;

	void OnResize(float width, float height) noexcept;

	void DrawLine(float x0, float y0, float x1, float y1, const Color& color, float strokeWidth = 1.0f) noexcept;

	void BeginDraw() noexcept;
	void EndDraw() noexcept;

	IDWriteFactory7*	 DWriteFactory() const { return m_dwriteFactory.Get(); }
	IWICImagingFactory2* WICImagingFactory() const { return m_wicImagingFactory.Get(); }
	ID2D1Factory7*		 D2DFactory() const { return m_d2dFactory.Get(); }
	ID2D1Device6*		 D2DDevice() const { return m_d2dDevice.Get(); }
	ID2D1DeviceContext6* D2DDeviceContext() const { return m_d2dDeviceContext.Get(); }
	ID2D1Bitmap1*		 D2DBitmap() const { return m_d2dBitmap.Get(); }

	/*
	static void SetViewport(D3D11_VIEWPORT viewport) noexcept { Get().SetViewportImpl(viewport); }

	static D3D11_VIEWPORT GetViewport() noexcept { return Get().m_viewport; }
	static ID3D11Device5* D3DDevice() noexcept { return Get().m_d3dDevice.Get(); }
	static ID3D11DeviceContext4* D3DDeviceContext() noexcept { return Get().m_d3dDeviceContext.Get(); }
	static ID3D11DepthStencilView* DepthStencilView() noexcept { return Get().m_d3dDepthStencilView.Get(); }
	static ID3D11RenderTargetView1* BackBufferRenderTargetView() noexcept { return Get().m_d3dRenderTargetView.Get(); }
	*/

#if defined(_DEBUG)
public:
	static DxgiInfoManagerDX11& GetInfoManager() noexcept { return m_infoManager; }
private:
	static DxgiInfoManagerDX11 m_infoManager;
#endif

	
private:
	void CreateDeviceDependentResources() noexcept;
	void CreateDeviceIndependentResources() noexcept;
	void CreateWindowSizeDependentResources() noexcept;
	void CreateWindowSizeDependentResources(float width, float height) noexcept;
	void SetRenderTarget() noexcept;
	void HandleDeviceLost() noexcept;


	// Keep handle to window
	HWND m_hWnd;

	// DPI scale for the window
	float m_dpiScale;

	// ==========================================================
	// Direct 2D drawing components
	Microsoft::WRL::ComPtr<ID2D1Factory7>		m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device6>		m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext6>	m_d2dDeviceContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>		m_d2dBitmap;

	Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_drawingStateBlock;

	// Direct Write drawing components
	Microsoft::WRL::ComPtr<IDWriteFactory7>		m_dwriteFactory;
	Microsoft::WRL::ComPtr<IWICImagingFactory2>	m_wicImagingFactory;

	// Direct3D objects
	Microsoft::WRL::ComPtr<ID3D11Device5>		 m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext4> m_d3dDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain4>		 m_dxgiSwapChain;

	// Direct3D Rendering objects
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView1>	m_d3dRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_d3dDepthStencilView;

	// Keep one rasterstate for solid fill and another for wireframe
	// Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidRasterState;
	// Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRasterState;

	D3D11_VIEWPORT m_viewport;

	// Cached device properties
	D3D_FEATURE_LEVEL m_d3dFeatureLevel;

	// Orientation transforms
	// D2D1::Matrix3x2F	m_orientationTransform2D;
	// DirectX::XMFLOAT4X4 m_orientationTransform3D;
};
#pragma warning( pop )

}

#endif // EG_DX11