#include "PipelineConfig.h"
#include "Structs.h"


using namespace Evergreen;
using namespace DirectX;

PipelineConfig::PipelineConfig(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
								std::unique_ptr<VertexShader> vertexShader,
								std::unique_ptr<PixelShader> pixelShader,
								std::unique_ptr<InputLayout> inputLayout,
								std::unique_ptr<RasterizerState> rasterizerState,
								std::unique_ptr<BlendState> blendState,
								std::unique_ptr<DepthStencilState> depthStencilState,
								std::unique_ptr<ConstantBufferArray> vertexShaderConstantBufferArray,
								std::unique_ptr<ConstantBufferArray> pixelShaderConstantBufferArray) :
	m_deviceResources(deviceResources),
	m_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_blendFactor{ 1.0f, 1.0f, 1.0f, 1.0f },
	m_blendSampleMask(0xffffffff),
	m_stencilRef(0u),
	m_vertexShader(std::move(vertexShader)),
	m_pixelShader(std::move(pixelShader)),
	m_inputLayout(std::move(inputLayout)),
	m_rasterizerState(std::move(rasterizerState)),
	m_blendState(std::move(blendState)),
	m_depthStencilState(std::move(depthStencilState)),
	m_vertexShaderConstantBufferArray(std::move(vertexShaderConstantBufferArray)),
	m_pixelShaderConstantBufferArray(std::move(pixelShaderConstantBufferArray))
{
	EG_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_ASSERT(m_vertexShader != nullptr, "cannot be nullptr");
	EG_ASSERT(m_pixelShader != nullptr, "cannot be nullptr");
	EG_ASSERT(m_inputLayout != nullptr, "cannot be nullptr");
	EG_ASSERT(m_rasterizerState != nullptr, "cannot be nullptr");
	EG_ASSERT(m_blendState != nullptr, "cannot be nullptr");
	EG_ASSERT(m_depthStencilState != nullptr, "cannot be nullptr");
}

void PipelineConfig::ApplyConfig() const
{
	EG_ASSERT(m_deviceResources != nullptr, "No device resources");

	auto context = m_deviceResources->D3DDeviceContext();

	// bind pixel shader
	GFX_THROW_INFO_ONLY(context->PSSetShader(m_pixelShader->Get(), nullptr, 0u));

	// bind vertex shader
	GFX_THROW_INFO_ONLY(context->VSSetShader(m_vertexShader->Get(), nullptr, 0u));

	// bind vertex layout
	GFX_THROW_INFO_ONLY(context->IASetInputLayout(m_inputLayout->Get()));

	// Set primitive topology to triangle list (groups of 3 vertices)
	GFX_THROW_INFO_ONLY(context->IASetPrimitiveTopology(m_topology));

	// Set Rasterizer State
	GFX_THROW_INFO_ONLY(context->RSSetState(m_rasterizerState->Get()));

	// Set Blend State
	GFX_THROW_INFO_ONLY(context->OMSetBlendState(m_blendState->Get(), m_blendFactor, m_blendSampleMask));

	// Set Depth Stencil State
	GFX_THROW_INFO_ONLY(context->OMSetDepthStencilState(m_depthStencilState->Get(), m_stencilRef));

	// Set the VS constant buffers
	if (m_vertexShaderConstantBufferArray != nullptr)
		m_vertexShaderConstantBufferArray->BindVS();

	// Set the PS constant buffers
	if (m_pixelShaderConstantBufferArray != nullptr)
		m_pixelShaderConstantBufferArray->BindPS();
}