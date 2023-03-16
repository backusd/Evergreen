#include <Evergreen.h>
#include "Utils/MathHelper.h"

using namespace Evergreen;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT4X4;
using DirectX::XMVECTOR;
using DirectX::XMMATRIX;
using DirectX::XMVectorSet;
using DirectX::XMVectorZero;
using DirectX::XMMatrixLookAtLH;
using DirectX::XMLoadFloat4x4;
using DirectX::XMStoreFloat4x4;
using DirectX::XMMatrixPerspectiveFovLH;

class Sandbox : public Evergreen::Application
{
public:
	Sandbox()
	{
		SetCallbacks();
		m_ui->SetUIRoot("src/json/");
		m_ui->LoadUI("main.json");
	}
	Sandbox(const Sandbox&) = delete;
	void operator=(const Sandbox&) = delete;
	~Sandbox() override {}
	
	

protected:
	XMFLOAT4X4 m_world = MathHelper::Identity4x4();
	XMFLOAT4X4 m_view = MathHelper::Identity4x4();
	XMFLOAT4X4 m_proj = MathHelper::Identity4x4();
	float m_theta = 1.5f * DirectX::XM_PI;
	float m_phi = DirectX::XM_PIDIV4;
	float m_radius = 5.0f;

	void OnUpdate(const Timer& timer) override
	{

	}
	void OnRender() override
	{
		auto context = m_deviceResources->D3DDeviceContext();
		auto device = m_deviceResources->D3DDevice();

		// bind render target
		ID3D11RenderTargetView* const targets[1] = { m_deviceResources->BackBufferRenderTargetView() };
		GFX_THROW_INFO_ONLY(context->OMSetRenderTargets(1u, targets, m_deviceResources->DepthStencilView()))

		float c = 92.0f / 256;
		float background[4] = { c, 0.0f, c, 1.0f };
		GFX_THROW_INFO_ONLY(context->ClearRenderTargetView(m_deviceResources->BackBufferRenderTargetView(), background))
		GFX_THROW_INFO_ONLY(context->ClearDepthStencilView(m_deviceResources->DepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0))


		struct Vertex
		{
			XMFLOAT3 Pos;
			XMFLOAT4 Color;
		};

		struct ObjectConstants
		{
			XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
		};

		std::array<Vertex, 8> vertices =
		{
			Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::White) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Black) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Red) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Green) }),
			Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Blue) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Yellow) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Cyan) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Magenta) })
		};

		std::array<std::uint16_t, 36> indices =
		{
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();
		GFX_THROW_INFO(device->CreateBuffer(&bd, &sd, &pVertexBuffer))

		// Bind vertex buffer to pipeline
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		GFX_THROW_INFO_ONLY(context->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset))


		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(std::uint16_t);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices.data();
		GFX_THROW_INFO(device->CreateBuffer(&ibd, &isd, &pIndexBuffer))

		GFX_THROW_INFO_ONLY(context->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u))



		// create pixel shader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
		GFX_THROW_INFO(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader))

		// bind pixel shader
		GFX_THROW_INFO_ONLY(context->PSSetShader(pPixelShader.Get(), nullptr, 0u))

		// create vertex shader
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
		GFX_THROW_INFO(device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader))

		// bind vertex shader
		GFX_THROW_INFO_ONLY(context->VSSetShader(pVertexShader.Get(), nullptr, 0u))



		// input (vertex) layout (2d position only)
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		GFX_THROW_INFO(device->CreateInputLayout(
			ied,
			(UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout
		))

		// bind vertex layout
		GFX_THROW_INFO_ONLY(context->IASetInputLayout(pInputLayout.Get()))

		// Set primitive topology to triangle list (groups of 3 vertices)
		GFX_THROW_INFO_ONLY(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST))




		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = sizeof(ObjectConstants);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		ObjectConstants object;
		// Convert Spherical to Cartesian coordinates.
		float x = m_radius * sinf(m_phi) * cosf(m_theta);
		float z = m_radius * sinf(m_phi) * sinf(m_theta);
		float y = m_radius * cosf(m_phi);

		// Build the view matrix.
		XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
		XMVECTOR target = XMVectorZero();
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
		XMStoreFloat4x4(&m_view, view);

		XMMATRIX world = XMLoadFloat4x4(&m_world);

		float aspectRatio = static_cast<float>(m_window->GetWidth()) / m_window->GetHeight();
		XMMATRIX projection = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_proj, projection);
		XMMATRIX proj = XMLoadFloat4x4(&m_proj);
		XMMATRIX worldViewProj = world * view * proj;

		// Update the constant buffer with the latest worldViewProj matrix.
		ObjectConstants objConstants;
		XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));

		D3D11_SUBRESOURCE_DATA bufferData;
		bufferData.pSysMem = static_cast<void*>(&objConstants);
		bufferData.SysMemPitch = 0u;
		bufferData.SysMemSlicePitch = 0u;

		GFX_THROW_INFO(device->CreateBuffer(&bufferDesc, &bufferData, buffer.ReleaseAndGetAddressOf()))

		ID3D11Buffer* const buffers[1] = { buffer.Get() };
		GFX_THROW_INFO_ONLY(context->VSSetConstantBuffers(0u, 1u, buffers))




		// configure viewport
		auto vp = m_ui->GetControlByName<Viewport>("MainViewport");
		GFX_THROW_INFO_ONLY(context->RSSetViewports(1, &vp->GetViewport()))

		// GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0u));
		//context->Draw((UINT)std::size(vertices), 0u);

		GFX_THROW_INFO_ONLY(context->DrawIndexed((UINT)std::size(indices), 0u, 0u))


		// --------------------------------------------------------------------------------------------
		/*
		struct Vertex
		{
			struct
			{
				float x;
				float y;
			} pos;
			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} color;
		};

		Vertex vertices[] =
		{
			{ 0.0f,0.5f,255,0,0,0 },
			{ 0.5f,-0.5f,0,255,0,0 },
			{ -0.5f,-0.5f,0,0,255,0 },
			{ -0.3f,0.3f,0,255,0,0 },
			{ 0.3f,0.3f,0,0,255,0 },
			{ 0.0f,-0.8f,255,0,0,0 },
		};
		vertices[0].color.g = 255;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		device->CreateBuffer(&bd, &sd, &pVertexBuffer);

		// Bind vertex buffer to pipeline
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		context->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		// create index buffer
		const unsigned short indices[] =
		{
			0,1,2,
			0,2,3,
			0,4,1,
			2,1,5,
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		device->CreateBuffer(&ibd, &isd, &pIndexBuffer);

		// bind index buffer
		context->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

		// create pixel shader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
		device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

		// bind pixel shader
		context->PSSetShader(pPixelShader.Get(), nullptr, 0u);

		// create vertex shader
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
		device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);

		// bind vertex shader
		context->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		// input (vertex) layout (2d position only)
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,8u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		device->CreateInputLayout(
			ied, 
			(UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout
		);

		// bind vertex layout
		context->IASetInputLayout(pInputLayout.Get());

		// bind render target
		ID3D11RenderTargetView* const targets[1] = { m_deviceResources->BackBufferRenderTargetView() };
		context->OMSetRenderTargets(1u, targets, m_deviceResources->DepthStencilView());

		float c = 92.0f / 256;
		float background[4] = { c, 0.0f, c, 1.0f };
		context->ClearRenderTargetView(m_deviceResources->BackBufferRenderTargetView(), background);

		context->ClearDepthStencilView(m_deviceResources->DepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Set primitive topology to triangle list (groups of 3 vertices)
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// configure viewport
		auto vp = m_ui->GetControlByName<Viewport>("MainViewport");
		context->RSSetViewports(1, &vp->GetViewport());

		// GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0u));
		context->Draw((UINT)std::size(vertices), 0u);
		*/
	}

private:
	void SetCallbacks()
	{
		// MenuBar Callbacks
		SetMenuBarCallbacks();
		SetMenuBarFileDropDownCallbacks();
		SetMenuBarEditDropDownCallbacks();
		SetMenuBarViewDropDownCallbacks();





		// TESTING ================================================================================
		// Text::OnUpdate
		JSONLoaders::AddOnUpdateCallback("SetFPSText",
			[](Control* c, const Timer& timer)
			{
				Text* text = static_cast<Text*>(c);
				text->SetText(std::format(L"{}", timer.GetFramesPerSecond()));
			}
		);
		
		
		
		
		// Viewport
		JSONLoaders::AddCallback("ViewportExample_OnChar",
			[](Viewport* vp, CharEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnKeyPressed",
			[](Viewport* vp, KeyPressedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnKeyReleased",
			[](Viewport* vp, KeyReleasedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnMouseEntered",
			[](Viewport* vp, MouseMoveEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnMouseExited",
			[](Viewport* vp, MouseMoveEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnMouseMoved",
			[](Viewport* vp, MouseMoveEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnMouseScrolledVertical",
			[](Viewport* vp, MouseScrolledEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnMouseScrolledHorizontal",
			[](Viewport* vp, MouseScrolledEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnMouseButtonPressed",
			[](Viewport* vp, MouseButtonPressedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnMouseButtonReleased",
			[](Viewport* vp, MouseButtonReleasedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnClick",
			[](Viewport* vp, MouseButtonReleasedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("ViewportExample_OnDoubleClick",
			[](Viewport* vp, MouseButtonDoubleClickEvent& e)
			{
			}
		);
		 
		 
		
		// RadioButton
		JSONLoaders::AddCallback("RadioButtonExample_OnMouseEntered",
			[](RadioButton* rb, MouseMoveEvent& e)
			{
				//std::unique_ptr<Evergreen::SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Red));
				//slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("RadioButtonExample_OnMouseExited",
			[](RadioButton* rb, MouseMoveEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("RadioButtonExample_OnMouseMoved",
			[](RadioButton* rb, MouseMoveEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("RadioButtonExample_OnMouseLButtonDown",
			[](RadioButton* rb, MouseButtonPressedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("RadioButtonExample_OnMouseLButtonUp",
			[](RadioButton* rb, MouseButtonReleasedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("RadioButtonExample_OnIsCheckedChanged",
			[](RadioButton* rb, RadioButtonIsCheckedChangedEvent& e)
			{
			}
		);
		
		// SliderFloat
		JSONLoaders::AddCallback("SliderFloatExample_OnMouseEnteredCircle", 
			[](SliderFloat* slider, MouseMoveEvent& e)
			{
				std::unique_ptr<Evergreen::SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Red));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnMouseExitedCircle", 
			[](SliderFloat* slider, MouseMoveEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Purple));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnBeginDragging", 
			[](SliderFloat* slider, MouseButtonPressedEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Green));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnStoppedDragging", 
			[](SliderFloat* slider, MouseButtonReleasedEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::White));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnValueChanged", 
			[](SliderFloat* slider, SliderFloatValueChangedEvent& e)
			{
				EG_TRACE("Slider: {}", slider->GetValue());
			}
		);

		// SliderInt
		JSONLoaders::AddCallback("SliderFloatExample_OnMouseEnteredCircle",
			[](SliderInt* slider, MouseMoveEvent& e)
			{
				std::unique_ptr<Evergreen::SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Red));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnMouseExitedCircle",
			[](SliderInt* slider, MouseMoveEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Purple));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnBeginDragging",
			[](SliderInt* slider, MouseButtonPressedEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Green));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnStoppedDragging",
			[](SliderInt* slider, MouseButtonReleasedEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::White));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnValueChanged",
			[](SliderInt* slider, SliderIntValueChangedEvent& e)
			{
				EG_TRACE("Slider: {}", slider->GetValue());
			}
		);

		// Pane
		JSONLoaders::AddCallback("Pane1_OnMouseEnteredTitleBar", 
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue));
				pane->SetTitleBarBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("Pane1_OnMouseExitedTitleBar",
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f));
				pane->SetTitleBarBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("Pane1_OnMouseEnteredContentRegion",
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::White));
				pane->SetBackgroundBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("Pane1_OnMouseExitedContentRegion",
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkGray));
				pane->SetBackgroundBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("Pane1_OnMouseMoved",
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::string newTitle = std::format("x: {}, y: {}", e.GetX(), e.GetY());
				pane->ClearTitleBarLayoutAndAddTitle(newTitle);
			}
		);

		// TextInput
		JSONLoaders::AddCallback("TextInput1_OnMouseEntered", 
			[](TextInput* textInput, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddCallback("TextInput1_OnMouseExited", 
			[](TextInput* textInput, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Gray));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddCallback("TextInput1_OnMouseMoved", 
			[](TextInput* textInput, MouseMoveEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("TextInput1_OnMouseLButtonDown", 
			[](TextInput* textInput, MouseButtonPressedEvent& e)
			{
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Coral));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddCallback("TextInput1_OnMouseLButtonUp", 
			[](TextInput* textInput, MouseButtonReleasedEvent& e)
			{
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddCallback("TextInput1_OnClick", 
			[](TextInput* textInput, MouseButtonReleasedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("TextInput1_OnEnterKey", 
			[](TextInput* textInput, CharEvent& e)
			{
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddCallback("TextInput1_OnInputTextChanged", 
			[](TextInput* textInput, CharEvent& e)
			{
			}
		);


		// Test Button
		JSONLoaders::AddCallback("TestButtonOnMouseEnter", 
			[](Button* button, MouseMoveEvent& e)
			{
				float c = 0.35f;
				std::vector<D2D1_GRADIENT_STOP> stops{ 
					{0.0f, D2D1::ColorF(c, c, c)}, 
					{0.4f, D2D1::ColorF(D2D1::ColorF::Black)},
					{1.0f, D2D1::ColorF(D2D1::ColorF::Black)}
				};

				std::unique_ptr<RadialBrush> brush = std::make_unique<RadialBrush>(
					button->GetDeviceResources(),
					stops,
					D2D1::Point2F()
				);

				brush->SetDrawRegionRectModifier(
					[](const D2D1_RECT_F& rect, ColorBrush* brush) -> D2D1_RECT_F
					{
						float height = rect.bottom - rect.top;
						float width = rect.right - rect.left;
						float centerX = rect.left + (width / 2.0f);
						float centerY = rect.top + (height / 2.0f);

						float halfLength = std::max(height, width);

						return D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength);
					}
				);

				button->BackgroundBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("TestButtonOnMouseLeave", 
			[](Button* button, MouseMoveEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Black))));
			}
		);		
		JSONLoaders::AddCallback("TestButtonOnMouseMoved", 
			[](Button* button, MouseMoveEvent& e)
			{
				D2D1_RECT_F rect = button->AllowedRegion();

				float height = rect.bottom - rect.top;
				float width = rect.right - rect.left;
				float halfLength = std::max(height, width);

				float centerX = e.GetX();
				float centerY = e.GetY();

				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
				backgroundBrush->SetDrawRegion(
					D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength)
				);

//				float centerX = rect.left + ((rect.right - rect.left) / 2.0f);
//				float centerY = rect.top + ((rect.bottom - rect.top) / 2.0f);
//
//				MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);
//				
//				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
//				backgroundBrush->GradientOriginOffset(D2D1::Point2F(mme.GetX() - centerX, mme.GetY() - centerY));
//
//				
//				
//				D2D1_RECT_F rect = button->AllowedRegion();
//				float centerX = rect.left + ((rect.right - rect.left) / 2.0f);
//				float centerY = rect.top + ((rect.bottom - rect.top) / 2.0f);
//
//				D2D1_POINT_2F mouseLocation = button->MouseLocation();
//
//				std::unique_ptr<RadialBrush> brush = std::make_unique<RadialBrush>(
//					button->GetDeviceResources(), 
//					D2D1::ColorF(D2D1::ColorF::Gainsboro),
//					D2D1::ColorF(D2D1::ColorF::Black),
//					D2D1::Point2F(mouseLocation.x - centerX, mouseLocation.y - centerY)
//				);
//
//				button->BackgroundBrush(std::move(brush));
//
//				//button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Orange))));
			}
		);
		JSONLoaders::AddCallback("TestButtonOnMouseLButtonDown", 
			[](Button* button, MouseButtonPressedEvent& e)
			{
				// button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Peru))));
			}
		);
		JSONLoaders::AddCallback("TestButtonOnMouseLButtonUp", 
			[](Button* button, MouseButtonReleasedEvent& e)
			{
				// Only need to change the background color if the mouse is still over the button (because if the mouse leaves the button area, the
				// OnMouseLeave event will fire and set the background color anyways)
				
				//if (button->MouseIsOver())
				//{
				//	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkOrange))));
				//}
			}
		);
		JSONLoaders::AddCallback("TestButtonOnClick", 
			[](Button* button, MouseButtonReleasedEvent& e)
			{
			}
		);

		// Test Button 2
		JSONLoaders::AddCallback("TestButton2OnMouseEnter", 
			[](Button* button, MouseMoveEvent& e)
			{
				std::vector<D2D1_GRADIENT_STOP> stops{
					{0.0f, D2D1::ColorF(D2D1::ColorF::Pink)},
					{0.2f, D2D1::ColorF(D2D1::ColorF::Blue)},
					{1.0f, D2D1::ColorF(D2D1::ColorF::Blue)}
				};

				std::unique_ptr<RadialBrush> brush = std::make_unique<RadialBrush>(
					button->GetDeviceResources(),
					stops,
					D2D1::Point2F()
					);

				brush->SetDrawRegionRectModifier(
					[](const D2D1_RECT_F& rect, ColorBrush* brush) -> D2D1_RECT_F
					{
						float height = rect.bottom - rect.top;
						float width = rect.right - rect.left;
						float centerX = rect.left + (width / 2.0f);
						float centerY = rect.top + (height / 2.0f);

						float halfLength = std::max(height, width);

						return D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength);
					}
				);

				button->BackgroundBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("TestButton2OnMouseLeave", 
			[](Button* button, MouseMoveEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue))));
			}
		);
		JSONLoaders::AddCallback("TestButton2OnMouseMoved", 
			[](Button* button, MouseMoveEvent& e)
			{
				D2D1_RECT_F rect = button->AllowedRegion();

				float height = rect.bottom - rect.top;
				float width = rect.right - rect.left;
				float halfLength = std::max(height, width);

				float centerX = e.GetX();
				float centerY = e.GetY();

				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
				backgroundBrush->SetDrawRegion(
					D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength)
				);

			}
		);
		JSONLoaders::AddCallback("TestButton2OnMouseLButtonDown", 
			[](Button* button, MouseButtonPressedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("TestButton2OnMouseLButtonUp", 
			[](Button* button, MouseButtonReleasedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("TestButton2OnClick", 
			[](Button* button, MouseButtonReleasedEvent& e)
			{
			}
		);

	}

	// MenuBar Callbacks
	void SetMenuBarCallbacks()
	{
		// File ----------------------------------------------------------------------------------------------
		JSONLoaders::AddCallback("FileDropDownButtonOnMouseEnter", 
			[this](Button* button, MouseMoveEvent& e)
			{
				Pane* filePane = m_ui->GetPane("FileDropDownPane");
				D2D1_COLOR_F color = filePane->GetVisible() ? m_menuBarButtonColorPaneOpen : m_menuBarButtonColorMouseOverPaneClosed;
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, color)));
			}
		);
		JSONLoaders::AddCallback("FileDropDownButtonOnMouseLeave", 
			[this](Button* button, MouseMoveEvent& e)
			{
				Pane* filePane = m_ui->GetPane("FileDropDownPane");

				// First if the file pane is visible, check if the mouse is now over the Edit button
				if (filePane->GetVisible())
				{
					Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
					if (editButton->ContainsPoint(e.GetX(), e.GetY()))
					{
						filePane->SetVisible(false);
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));

						Pane* editPane = m_ui->GetPane("EditDropDownPane");
						editPane->SetVisible(true);
					}
					else if (filePane->ContainsPoint(e.GetX(), e.GetY()))
					{
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
					}
					else
					{
						// Mouse is not over the button or pane, so close the pane
						filePane->SetVisible(false);
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					}
				}
				else
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
				}
			}
		);
		JSONLoaders::AddCallback("FileDropDownButtonOnMouseLButtonDown", 
			[this](Button* button, MouseButtonPressedEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
			}
		);
		JSONLoaders::AddCallback("FileDropDownButtonOnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				Pane* filePane = m_ui->GetPane("FileDropDownPane");
				filePane->SwitchVisible();

				if (filePane->GetVisible())
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				}
				else if (button->MouseIsOver())
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				}
			}
		);

		// Edit ----------------------------------------------------------------------------------------------
		JSONLoaders::AddCallback("EditDropDownButtonOnMouseEnter", 
			[this](Button* button, MouseMoveEvent& e)
			{
				Pane* editPane = m_ui->GetPane("EditDropDownPane");
				D2D1_COLOR_F color = editPane->GetVisible() ? m_menuBarButtonColorPaneOpen : m_menuBarButtonColorMouseOverPaneClosed;
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, color)));
			}
		);
		JSONLoaders::AddCallback("EditDropDownButtonOnMouseLeave",
			[this](Button* button, MouseMoveEvent& e)
			{
				Pane* editPane = m_ui->GetPane("EditDropDownPane");

				// First if the edit pane is visible, check if the mouse is now over the File button or View Button
				if (editPane->GetVisible())
				{
					Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
					Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");
					MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);
					if (fileButton->ContainsPoint(mme.GetX(), mme.GetY()))
					{
						editPane->SetVisible(false);
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));

						Pane* filePane = m_ui->GetPane("FileDropDownPane");
						filePane->SetVisible(true);
					}
					else if (viewButton->ContainsPoint(mme.GetX(), mme.GetY()))
					{
						editPane->SetVisible(false);
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));

						Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
						viewPane->SetVisible(true);
					}
					else if (editPane->ContainsPoint(mme.GetX(), mme.GetY()))
					{
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
					}
					else
					{
						// Mouse is not over the button or pane, so close the pane
						editPane->SetVisible(false);
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					}
				}
				else
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
				}
			}
		);
		JSONLoaders::AddCallback("EditDropDownButtonOnMouseLButtonDown", 
			[this](Button* button, MouseButtonPressedEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
			}
		);
		JSONLoaders::AddCallback("EditDropDownButtonOnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				Pane* editPane = m_ui->GetPane("EditDropDownPane");
				editPane->SwitchVisible();

				if (editPane->GetVisible())
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				}
				else if (button->MouseIsOver())
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				}
			}
		);

		// View ----------------------------------------------------------------------------------------------
		JSONLoaders::AddCallback("ViewDropDownButtonOnMouseEnter", 
			[this](Button* button, MouseMoveEvent& e)
			{
				Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
				D2D1_COLOR_F color = viewPane->GetVisible() ? m_menuBarButtonColorPaneOpen : m_menuBarButtonColorMouseOverPaneClosed;
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, color)));
			}
		);
		JSONLoaders::AddCallback("ViewDropDownButtonOnMouseLeave", 
			[this](Button* button, MouseMoveEvent& e)
			{
				Pane* viewPane = m_ui->GetPane("ViewDropDownPane");

				// First if the view pane is visible, check if the mouse is now over the Edit button
				if (viewPane->GetVisible())
				{
					Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
					MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);
					if (editButton->ContainsPoint(mme.GetX(), mme.GetY()))
					{
						viewPane->SetVisible(false);
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));

						Pane* editPane = m_ui->GetPane("EditDropDownPane");
						editPane->SetVisible(true);
					}
					else if (viewPane->ContainsPoint(mme.GetX(), mme.GetY()))
					{
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
					}
					else
					{
						// Mouse is not over the button or pane, so close the pane
						viewPane->SetVisible(false);
						button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					}
				}
				else
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
				}
			}
		);
		JSONLoaders::AddCallback("ViewDropDownButtonOnMouseLButtonDown",
			[this](Button* button, MouseButtonPressedEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
			}
		);
		JSONLoaders::AddCallback("ViewDropDownButtonOnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
				viewPane->SwitchVisible();

				if (viewPane->GetVisible())
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				}
				else if (button->MouseIsOver())
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				}
			}
		);

		// Pane ----------------------------------------------------------------------------------------------
		JSONLoaders::AddCallback("MenuBarDropDownPaneOnMouseExitedContentRegion", 
			[this](Pane* pane, MouseMoveEvent& e)
			{
				Pane* filePane = m_ui->GetPane("FileDropDownPane");
				Pane* editPane = m_ui->GetPane("EditDropDownPane");
				Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
				Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
				Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
				Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");

				if (fileButton->ContainsPoint(e.GetX(), e.GetY()))
				{
					if (filePane != pane)
					{
						pane->SetVisible(false);
						filePane->SetVisible(true);

						if (pane == editPane)
						{
							editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						}
						else if (pane == viewPane)
						{
							viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						}
					}
				}
				else if (editButton->ContainsPoint(e.GetX(), e.GetY()))
				{
					if (editPane != pane)
					{
						pane->SetVisible(false);
						editPane->SetVisible(true);

						if (pane == filePane)
						{
							fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						}
						else if (pane == viewPane)
						{
							viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						}
					}
				}
				else if (viewButton->ContainsPoint(e.GetX(), e.GetY()))
				{
					if (viewPane != pane)
					{
						pane->SetVisible(false);
						viewPane->SetVisible(true);

						if (pane == filePane)
						{
							fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						}
						else if (pane == editPane)
						{
							editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						}
					}
				}
				else
				{
					// Mouse is not over pane or menu bar button, so collapse it
					pane->SetVisible(false);
					if (pane == filePane)
						fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					else if (pane == editPane)
						editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					else if (pane == viewPane)
						viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
				}
			}
		);
	}
	void SetMenuBarFileDropDownCallbacks()
	{
		// EACH Item
		JSONLoaders::AddCallback("FileDropDown_Item_OnMouseEnter", 
			[this](Button* button, MouseMoveEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
			}
		);
		JSONLoaders::AddCallback("FileDropDown_Item_OnMouseExited",
			[this](Button* button, MouseMoveEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
			}
		);

		// New
		JSONLoaders::AddCallback("FileDropDown_NewButton_OnClick",
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* filePane = m_ui->GetPane("FileDropDownPane");
				filePane->SetVisible(false);
				Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
				fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);

		// Open
		JSONLoaders::AddCallback("FileDropDown_OpenButton_OnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* filePane = m_ui->GetPane("FileDropDownPane");
				filePane->SetVisible(false);
				Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
				fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);

		// Save
		JSONLoaders::AddCallback("FileDropDown_SaveButton_OnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* filePane = m_ui->GetPane("FileDropDownPane");
				filePane->SetVisible(false);
				Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
				fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);

		// Save As
		JSONLoaders::AddCallback("FileDropDown_SaveAsButton_OnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* filePane = m_ui->GetPane("FileDropDownPane");
				filePane->SetVisible(false);
				Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
				fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);

		// Close
		JSONLoaders::AddCallback("FileDropDown_CloseButton_OnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* filePane = m_ui->GetPane("FileDropDownPane");
				filePane->SetVisible(false);
				Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
				fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);
	}
	void SetMenuBarEditDropDownCallbacks()
	{
		// Edit 1
		JSONLoaders::AddCallback("EditDropDown_Edit1Button_OnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* editPane = m_ui->GetPane("EditDropDownPane");
				editPane->SetVisible(false);
				Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
				editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);

		// Edit 2
		JSONLoaders::AddCallback("EditDropDown_Edit2Button_OnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* editPane = m_ui->GetPane("EditDropDownPane");
				editPane->SetVisible(false);
				Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
				editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);
	}
	void SetMenuBarViewDropDownCallbacks()
	{
		// View 1
		JSONLoaders::AddCallback("ViewDropDown_View1Button_OnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
				viewPane->SetVisible(false);
				Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");
				viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);

		// View 2
		JSONLoaders::AddCallback("ViewDropDown_View2Button_OnClick", 
			[this](Button* button, MouseButtonReleasedEvent& e)
			{
				// Close the pane
				Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
				viewPane->SetVisible(false);
				Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");
				viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		);
	}

	const D2D1_COLOR_F m_menuBarButtonColorDefault = D2D1::ColorF(0.16f, 0.16f, 0.16f);
	const D2D1_COLOR_F m_menuBarButtonColorMouseOverPaneClosed = D2D1::ColorF(0.35f, 0.35f, 0.35f);
	const D2D1_COLOR_F m_menuBarButtonColorPaneOpen = D2D1::ColorF(0.25f, 0.25f, 0.25f);

};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Sandbox();
}