#include "Scene.h"

using namespace Evergreen;
using namespace DirectX;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources),
	m_currentCamera(0u),
	m_aspectRatio(1.0f)
{
	m_cameras.emplace_back();

	// Create the Pass Constants buffer so we can share this will ALL pipeline configurations
	std::shared_ptr<ConstantBuffer> vsPassConstantsBuffer = std::make_shared<ConstantBuffer>(deviceResources);
	vsPassConstantsBuffer->CreateBuffer<PassConstants>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	m_vsPerPassConstantsBuffers.push_back(vsPassConstantsBuffer);
	
	std::shared_ptr<ConstantBuffer> psPassConstantsBuffer = std::make_shared<ConstantBuffer>(deviceResources);
	psPassConstantsBuffer->CreateBuffer<PassConstants>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	m_psPerPassConstantsBuffers.push_back(psPassConstantsBuffer);

	// Create the buffer with all materials
	std::shared_ptr<ConstantBuffer> psMaterialsArrayBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	MaterialsArray materials;
	materials.materials[0].DiffuseAlbedo = XMFLOAT4(0.2f, 0.6f, 0.2f, 1.0f);
	materials.materials[0].FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	materials.materials[0].Shininess = 0.875;
	materials.materials[1].DiffuseAlbedo = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	materials.materials[1].FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	materials.materials[1].Shininess = 0.875f;
	psMaterialsArrayBuffer->CreateBuffer<MaterialsArray>(D3D11_USAGE_DEFAULT, 0u, 0u, 0u, &materials);

	m_psPerPassConstantsBuffers.push_back(psMaterialsArrayBuffer);

	// Pipeline Configuration 
	std::unique_ptr<PipelineConfig> config = std::make_unique<PipelineConfig>(m_deviceResources, m_vsPerPassConstantsBuffers, m_psPerPassConstantsBuffers);

	// Mesh Set
	std::unique_ptr<MeshSet> ms = std::make_unique<MeshSet>(m_deviceResources);
	ms->SetVertexConversionFunction([this](std::vector<MeshSet::GeneralVertex> input) -> std::vector<Vertex>
		{
			std::vector<Vertex> output(input.size());
			for (unsigned int iii = 0; iii < input.size(); ++iii)
			{
				output[iii].Pos = input[iii].Position;
				output[iii].Normal = input[iii].Normal;
			}
			return output;
		}
	);

	MeshInstance mi = ms->AddGeosphere(1.0f, 3);
	ms->Finalize();

	m_position1[0] = 0.0f;
	m_position1[1] = 0.0f;
	m_position1[2] = 0.0f;
	m_position2[0] = 2.5f;
	m_position2[1] = 0.0f;
	m_position2[2] = 0.0f;

	std::vector<RenderObjectList> objectLists;
	objectLists.emplace_back(deviceResources, mi);
	objectLists.back().AddRenderObject({ 1.0f, 1.0f, 1.0f }, m_position1);
	objectLists.back().AddRenderObject({ 1.0f, 1.0f, 1.0f }, m_position2);

	m_configsAndObjectLists.push_back(std::make_tuple(std::move(config), std::move(ms), objectLists));
}

void Scene::Update(const Timer& timer)
{
	auto context = m_deviceResources->D3DDeviceContext();

	// Update Pass Constants -----------------------------------------------------------------

	XMMATRIX view = m_cameras[m_currentCamera].GetViewMatrix();
	XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, m_aspectRatio, 1.0f, 1000.0f);

	XMVECTOR viewDet = DirectX::XMMatrixDeterminant(view);
	XMMATRIX invView = DirectX::XMMatrixInverse(&viewDet, view);

	XMVECTOR projDet = DirectX::XMMatrixDeterminant(proj);
	XMMATRIX invProj = DirectX::XMMatrixInverse(&projDet, proj);
	
	XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
	XMVECTOR viewProjDet = DirectX::XMMatrixDeterminant(viewProj);
	XMMATRIX invViewProj = DirectX::XMMatrixInverse(&viewProjDet, viewProj);

	DirectX::XMStoreFloat4x4(&m_passConstants.View, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4(&m_passConstants.InvView, DirectX::XMMatrixTranspose(invView));
	DirectX::XMStoreFloat4x4(&m_passConstants.Proj, DirectX::XMMatrixTranspose(proj));
	DirectX::XMStoreFloat4x4(&m_passConstants.InvProj, DirectX::XMMatrixTranspose(invProj));
	DirectX::XMStoreFloat4x4(&m_passConstants.ViewProj, DirectX::XMMatrixTranspose(viewProj));
	DirectX::XMStoreFloat4x4(&m_passConstants.InvViewProj, DirectX::XMMatrixTranspose(invViewProj));
	m_passConstants.EyePosW = m_cameras[m_currentCamera].GetPosition();
	m_passConstants.RenderTargetSize = XMFLOAT2(m_deviceResources->GetRenderTargetWidth(), m_deviceResources->GetRenderTargetHeight());
	m_passConstants.InvRenderTargetSize = XMFLOAT2(1.0f / m_deviceResources->GetRenderTargetWidth(), 1.0f / m_deviceResources->GetRenderTargetHeight());
	m_passConstants.NearZ = 1.0f;
	m_passConstants.FarZ = 1000.0f;
	m_passConstants.TotalTime = static_cast<float>(timer.GetTotalSeconds());
	m_passConstants.DeltaTime = static_cast<float>(timer.GetElapsedSeconds());
	m_passConstants.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

	XMVECTOR lightDir = -MathHelper::SphericalToCartesian(1.0f, 1.25f * XM_PI, XM_PIDIV4);
	DirectX::XMStoreFloat3(&m_passConstants.Lights[0].Direction, lightDir);
	m_passConstants.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };

	// Update the constant buffer with the new matrix values
	// VS
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	GFX_THROW_INFO(context->Map(m_vsPerPassConstantsBuffers[0]->GetRawBufferPointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_passConstants, sizeof(PassConstants));
	GFX_THROW_INFO_ONLY(context->Unmap(m_vsPerPassConstantsBuffers[0]->GetRawBufferPointer(), 0));

	// PS
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
	GFX_THROW_INFO(context->Map(m_psPerPassConstantsBuffers[0]->GetRawBufferPointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_passConstants, sizeof(PassConstants));
	GFX_THROW_INFO_ONLY(context->Unmap(m_psPerPassConstantsBuffers[0]->GetRawBufferPointer(), 0));


	// Update all render object lists
	for (auto& configAndObjectList : m_configsAndObjectLists)
	{
		std::vector<RenderObjectList>& objectLists = std::get<2>(configAndObjectList);
		for (unsigned int iii = 0; iii < objectLists.size(); ++iii)
		{
			objectLists[iii].Update(timer);
		}
	}
}

void Scene::Render()
{
	EG_ASSERT(m_currentCamera < m_cameras.size(), "The selected camera index must be within the bounds of the m_cameras vector");

	auto context = m_deviceResources->D3DDeviceContext();

	// Apply the pipeline config for each list of render objects, render each object, then move onto the next config
	for (auto& configAndObjectList : m_configsAndObjectLists)
	{
		std::unique_ptr<PipelineConfig>& config = std::get<0>(configAndObjectList);
		config->ApplyConfig();

		std::unique_ptr<MeshSet>& ms = std::get<1>(configAndObjectList);
		ms->BindToIA();

		std::vector<RenderObjectList>& objectLists = std::get<2>(configAndObjectList);
		for (unsigned int iii = 0; iii < objectLists.size(); ++iii)
		{
			objectLists[iii].Render();
		}
	}
}
