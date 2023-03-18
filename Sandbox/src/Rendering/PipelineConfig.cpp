#include "PipelineConfig.h"


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


PipelineConfig::PipelineConfig(std::shared_ptr<Evergreen::DeviceResources> deviceResources) :
	m_deviceResources(deviceResources)
{
}

void PipelineConfig::ApplyConfig()
{
	auto context = m_deviceResources->D3DDeviceContext();
	auto device = m_deviceResources->D3DDevice();

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

	XMMATRIX projection = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, m_aspectRatio, 1.0f, 1000.0f);
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






		GFX_THROW_INFO_ONLY(context->DrawIndexed((UINT)std::size(indices), 0u, 0u))
}