#include "Scene.h"

using namespace Evergreen;
using namespace DirectX;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources),
	m_currentCamera(0u),
	m_vsPassConstantsBuffer(nullptr),
	m_aspectRatio(1.0f)
{
	m_cameras.emplace_back();

	// Create the Pass Constants buffer so we can share this will ALL pipeline configurations
	m_vsPassConstantsBuffer = std::make_shared<ConstantBuffer>(deviceResources);
	m_vsPassConstantsBuffer->CreateBuffer<PassConstants>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);
	
	m_psPassConstantsBuffer = std::make_shared<ConstantBuffer>(deviceResources);
	m_psPassConstantsBuffer->CreateBuffer<PassConstants>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	// HILLS -----------------------------------------------------------------------------
	// Pipeline Configuration 
	std::unique_ptr<PipelineConfig> config = std::make_unique<PipelineConfig>(m_deviceResources, m_vsPassConstantsBuffer, m_psPassConstantsBuffer);

	std::unique_ptr<MeshSet> ms = std::make_unique<MeshSet>(m_deviceResources);

//	ms->SetVertexConversionFunction([](std::vector<MeshSet::GeneralVertex> input) -> std::vector<Vertex>
//		{
//			std::vector<Vertex> output(input.size());
//			for (unsigned int iii = 0; iii < input.size(); ++iii)
//			{
//				output[iii].Pos = input[iii].Position;
//				output[iii].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
//			}
//			return output;
//		}
//	);
	ms->SetVertexConversionFunction([this](std::vector<MeshSet::GeneralVertex> input) -> std::vector<Vertex>
		{
			std::vector<Vertex> output(input.size());
			for (unsigned int iii = 0; iii < input.size(); ++iii)
			{
				output[iii].Pos = input[iii].Position;
				output[iii].Pos.y = this->GetHillHeight(output[iii].Pos.x, output[iii].Pos.z);
				output[iii].Normal = this->GetHillNormal(output[iii].Pos.x, output[iii].Pos.z);
			}
			return output;
		}
	);

	//MeshInstance mi = ms->AddBox(1.0f, 1.0f, 1.0f, 0);
	//MeshInstance mi = ms->AddSphere(1.0f, 10, 10);
	//MeshInstance miSphere = ms->AddGeosphere(1.0f, 2);
	//MeshInstance mi = ms->AddCylinder(1.0f, 2.0f, 1.5f, 100, 100);
	MeshInstance miGrid = ms->AddGrid(160.0f, 160.0f, 50, 50);
	
	ms->Finalize();

	Material grass;
	grass.DiffuseAlbedo = XMFLOAT4(0.2f, 0.6f, 0.2f, 1.0f);
	grass.FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	grass.Roughness = 0.125f;
	std::vector<RenderObject> objects;
	objects.emplace_back(deviceResources, miGrid, grass);

	m_configsAndObjectLists.push_back(std::make_tuple(std::move(config), std::move(ms), objects));


	// WAVES -----------------------------------------------------------------------------
	// Pipeline Configuration 
	std::unique_ptr<PipelineConfig> waveConfig = std::make_unique<PipelineConfig>(m_deviceResources, m_vsPassConstantsBuffer, m_psPassConstantsBuffer);

	std::unique_ptr<MeshSet> waveMS = std::make_unique<MeshSet>(m_deviceResources, true);

	// Create Waves
	m_waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	std::vector<std::uint16_t> indices(3 * m_waves->TriangleCount()); // 3 indices per face
	int m = m_waves->RowCount();
	int n = m_waves->ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}
	std::vector<Vertex> vertices(m_waves->VertexCount());
	m_wavesMesh = waveMS->AddMesh(vertices, indices);

	waveMS->Finalize();

	Material water;
	water.DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
	water.FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water.Roughness = 0.0f;
	std::vector<RenderObject> waveObjects;
	waveObjects.emplace_back(deviceResources, m_wavesMesh, water);

	m_configsAndObjectLists.push_back(std::make_tuple(std::move(waveConfig), std::move(waveMS), waveObjects));
}

void Scene::Update(const Timer& timer)
{
	auto context = m_deviceResources->D3DDeviceContext();

	// Update Waves ----------------------------------------------------------------
	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if ((timer.GetTotalSeconds() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::Rand(4, m_waves->RowCount() - 5);
		int j = MathHelper::Rand(4, m_waves->ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.5f);

		m_waves->Disturb(i, j, r);
	}

	// Update the wave simulation.
	m_waves->Update(timer.GetElapsedSeconds());

	// Update the wave vertex buffer with the new solution.
	std::vector<Vertex> vertices(m_waves->VertexCount());
	for (int iii = 0; iii < m_waves->VertexCount(); ++iii)
	{
		vertices[iii].Pos = m_waves->Position(iii);
		vertices[iii].Normal = m_waves->Normal(iii);
	}

	std::unique_ptr<MeshSet>& meshSet = std::get<1>(m_configsAndObjectLists[1]);
	meshSet->UpdateVertices(vertices);

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
	XMStoreFloat3(&m_passConstants.Lights[0].Direction, lightDir);
	m_passConstants.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };

	// Update the constant buffer with the new matrix values
	// VS
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	GFX_THROW_INFO(context->Map(m_vsPassConstantsBuffer->GetRawBufferPointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_passConstants, sizeof(PassConstants));
	GFX_THROW_INFO_ONLY(context->Unmap(m_vsPassConstantsBuffer->GetRawBufferPointer(), 0));

	// PS
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
	GFX_THROW_INFO(context->Map(m_psPassConstantsBuffer->GetRawBufferPointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_passConstants, sizeof(PassConstants));
	GFX_THROW_INFO_ONLY(context->Unmap(m_psPassConstantsBuffer->GetRawBufferPointer(), 0));


	// Update all render objects
	for (auto& configAndObjectList : m_configsAndObjectLists)
	{
		std::vector<RenderObject>& objects = std::get<2>(configAndObjectList);
		for (unsigned int iii = 0; iii < objects.size(); ++iii)
		{
			objects[iii].Update(timer);
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

		std::vector<RenderObject>& objects = std::get<2>(configAndObjectList);
		for (unsigned int iii = 0; iii < objects.size(); ++iii)
		{
			objects[iii].Render();
		}
	}
}

DirectX::XMFLOAT3 Scene::GetHillNormal(float x, float z) const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = DirectX::XMVector3Normalize(XMLoadFloat3(&n));
	DirectX::XMStoreFloat3(&n, unitNormal);
	return n;
}