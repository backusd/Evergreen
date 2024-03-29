#include "PipelineConfig.h"
#include "Structs.h"


using namespace Evergreen;
using namespace DirectX;



PipelineConfig::PipelineConfig(std::shared_ptr<Evergreen::DeviceResources> deviceResources, 
	std::shared_ptr<ConstantBuffer> vsPassConstants,
	std::shared_ptr<ConstantBuffer> psPassConstants) :
		m_deviceResources(deviceResources),
		m_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
		m_blendFactor{1.0f, 1.0f, 1.04, 1.0f},
		m_blendSampleMask(0xffffffff),
		m_stencilRef(0u),
		m_vsConstantBuffers(deviceResources),
		m_psConstantBuffers(deviceResources)
{
	auto device = m_deviceResources->D3DDevice();

	// Create Pixel Shader
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	GFX_THROW_INFO(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pixelShader));

	// Create Vertex Shader
	D3DReadFileToBlob(L"VertexShader.cso", pBlob.ReleaseAndGetAddressOf());
	GFX_THROW_INFO(device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_vertexShader));

	// Create Input Layout
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GFX_THROW_INFO(device->CreateInputLayout(
		ied,
		(UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&m_inputLayout
	));

	// Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterDesc; // Fill with default values for now
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	GFX_THROW_INFO(device->CreateRasterizerState(&rasterDesc, m_rasterizerState.ReleaseAndGetAddressOf()));

	// Create Blend State (Use default values)
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	GFX_THROW_INFO(device->CreateBlendState(&blendDesc, m_blendState.ReleaseAndGetAddressOf()));

	// Create DepthStencil State (Use default values)
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	GFX_THROW_INFO(device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.ReleaseAndGetAddressOf()));

	// Populate the VS Constant Buffer Array
	std::shared_ptr<ConstantBuffer> vsModelBuffer = std::make_shared<ConstantBuffer>(deviceResources);
	vsModelBuffer->CreateBuffer<ObjectConstants>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	std::shared_ptr<ConstantBuffer> vsMaterialBuffer = std::make_shared<ConstantBuffer>(deviceResources);
	vsMaterialBuffer->CreateBuffer<Material>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	m_vsConstantBuffers.AddBuffer(vsPassConstants);	 // Buffer at slot 0
	m_vsConstantBuffers.AddBuffer(vsMaterialBuffer); // Buffer at slot 1
	m_vsConstantBuffers.AddBuffer(vsModelBuffer);	 // Buffer at slot 2

	// Populate the PS Constant Buffer Array
	std::shared_ptr<ConstantBuffer> psMaterialBuffer = std::make_shared<ConstantBuffer>(deviceResources);
	psMaterialBuffer->CreateBuffer<Material>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	m_psConstantBuffers.AddBuffer(psPassConstants);	 // Buffer at slot 0
	m_psConstantBuffers.AddBuffer(psMaterialBuffer); // Buffer at slot 1
}

void PipelineConfig::ApplyConfig() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	// bind pixel shader
	GFX_THROW_INFO_ONLY(context->PSSetShader(m_pixelShader.Get(), nullptr, 0u));

	// bind vertex shader
	GFX_THROW_INFO_ONLY(context->VSSetShader(m_vertexShader.Get(), nullptr, 0u));

	// bind vertex layout
	GFX_THROW_INFO_ONLY(context->IASetInputLayout(m_inputLayout.Get()));

	// Set primitive topology to triangle list (groups of 3 vertices)
	GFX_THROW_INFO_ONLY(context->IASetPrimitiveTopology(m_topology));

	// Set Rasterizer State
	GFX_THROW_INFO_ONLY(context->RSSetState(m_rasterizerState.Get()));

	// Set Blend State
	GFX_THROW_INFO_ONLY(context->OMSetBlendState(m_blendState.Get(), m_blendFactor, m_blendSampleMask));

	// Set Depth Stencil State
	GFX_THROW_INFO_ONLY(context->OMSetDepthStencilState(m_depthStencilState.Get(), m_stencilRef));

	// Set the VS constant buffers
	m_vsConstantBuffers.BindVS();

	// Set the PS constant buffers
	m_psConstantBuffers.BindPS();
}