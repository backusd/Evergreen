#pragma once
#include "pch.h"
#include "../Utils/MathHelper.h"
#include <Evergreen.h>
#include "ConstantBufferArray.h"
#include "Structs.h"



class PipelineConfig
{
public:
	PipelineConfig(std::shared_ptr<Evergreen::DeviceResources> deviceResources, 
		const std::vector<std::shared_ptr<ConstantBuffer>>& vsPerPassConstantBuffers,
		const std::vector<std::shared_ptr<ConstantBuffer>>& psPerPassConstantBuffers);
	void ApplyConfig() const;

private:
	void Initialize(std::shared_ptr<ConstantBuffer> vsPassConstants,
		std::shared_ptr<ConstantBuffer> psPassConstants);
	void InitializeInstanced(
		const std::vector<std::shared_ptr<ConstantBuffer>>& vsPerPassConstantBuffers,
		const std::vector<std::shared_ptr<ConstantBuffer>>& psPerPassConstantBuffers);

	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	D3D11_PRIMITIVE_TOPOLOGY						m_topology;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState>		m_blendState;
	float                                           m_blendFactor[4];
	unsigned int                                    m_blendSampleMask;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	unsigned int									m_stencilRef;

	ConstantBufferArray	m_vsConstantBuffers;
	ConstantBufferArray	m_psConstantBuffers;
};