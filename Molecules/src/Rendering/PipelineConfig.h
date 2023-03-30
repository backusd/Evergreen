#pragma once
#include "pch.h"
#include "../Utils/MathHelper.h"
#include <Evergreen.h>
#include "ConstantBufferArray.h"
#include "Structs.h"

#include "Shaders.h"
#include "InputLayout.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"


class PipelineConfig
{
public:
	PipelineConfig(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
		std::unique_ptr<VertexShader> vertexShader,
		std::unique_ptr<PixelShader> pixelShader,
		std::unique_ptr<InputLayout> inputLayout,
		std::unique_ptr<RasterizerState> rasterizerState,
		std::unique_ptr<BlendState> blendState,
		std::unique_ptr<DepthStencilState> depthStencilState,
		std::unique_ptr<ConstantBufferArray> vertexShaderConstantBufferArray,
		std::unique_ptr<ConstantBufferArray> pixelShaderConstantBufferArray);

	void ApplyConfig() const;

	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept { m_topology = topology; }
	void SetBlendFactor(float blendFactors[4]) noexcept { memcpy(m_blendFactor, blendFactors, sizeof(float) * 4); }
	void SetBlendSampleMask(unsigned int mask) noexcept { m_blendSampleMask = mask; }
	void SetStencilRef(unsigned int ref) noexcept { m_stencilRef = ref; }

private:
//	void Initialize(std::shared_ptr<ConstantBuffer> vsPassConstants,
//		std::shared_ptr<ConstantBuffer> psPassConstants);
//	void InitializeInstanced(
//		const std::vector<std::shared_ptr<ConstantBuffer>>& vsPerPassConstantBuffers,
//		const std::vector<std::shared_ptr<ConstantBuffer>>& psPerPassConstantBuffers);

	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	std::unique_ptr<VertexShader>		m_vertexShader;
	std::unique_ptr<PixelShader>		m_pixelShader;
	std::unique_ptr<InputLayout>		m_inputLayout;
	std::unique_ptr<RasterizerState>	m_rasterizerState;
	std::unique_ptr<BlendState>			m_blendState;
	std::unique_ptr<DepthStencilState>	m_depthStencilState;
	std::unique_ptr<ConstantBufferArray> m_vertexShaderConstantBufferArray;
	std::unique_ptr<ConstantBufferArray> m_pixelShaderConstantBufferArray;

	D3D11_PRIMITIVE_TOPOLOGY						m_topology;
	float                                           m_blendFactor[4];
	unsigned int                                    m_blendSampleMask;
	unsigned int									m_stencilRef;
};