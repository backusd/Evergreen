#include "PipelineConfig.h"
#include "Structs.h"


using namespace Evergreen;
using namespace DirectX;



PipelineConfig::PipelineConfig(std::shared_ptr<Evergreen::DeviceResources> deviceResources) :
	m_deviceResources(deviceResources)
{
}

void PipelineConfig::ApplyConfig()
{
	auto context = m_deviceResources->D3DDeviceContext();
	auto device = m_deviceResources->D3DDevice();



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
}