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

	std::unique_ptr<PipelineConfig> config = std::make_unique<PipelineConfig>(m_deviceResources);

	std::unique_ptr<MeshSet> ms = std::make_unique<MeshSet>(m_deviceResources);
	ms->SetVertexConversionFunction([](std::vector<MeshSet::GeneralVertex> input) -> std::vector<Vertex>
		{
			std::vector<Vertex> output(input.size());
			for (unsigned int iii = 0; iii < input.size(); ++iii)
			{
				output[iii].Pos = input[iii].Position;
				output[iii].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
			}
			return output;
		}
	);

	//MeshInstance mi = ms->AddBox(1.0f, 1.0f, 1.0f, 0);
	//MeshInstance mi = ms->AddSphere(1.0f, 10, 10);
	MeshInstance miSphere = ms->AddGeosphere(1.0f, 2);
	//MeshInstance mi = ms->AddCylinder(1.0f, 2.0f, 1.5f, 100, 100);
	MeshInstance miGrid = ms->AddGrid(4.0f, 4.0f, 4, 4);
	ms->Finalize();


	std::vector<RenderObject> objects;
	objects.emplace_back(deviceResources, miSphere);
	objects.emplace_back(deviceResources, miGrid);

	m_configAndObjectList = std::make_tuple(std::move(config), std::move(ms), objects);


	// Model Buffer
	auto device = m_deviceResources->D3DDevice();

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(ObjectConstants);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	GFX_THROW_INFO(device->CreateBuffer(&bufferDesc, nullptr, m_vsModelBuffer.ReleaseAndGetAddressOf()));

	// Pass Constants Buffer
	D3D11_BUFFER_DESC bufferDesc2;
	bufferDesc2.ByteWidth = sizeof(PassConstants);
	bufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc2.MiscFlags = 0;
	bufferDesc2.StructureByteStride = 0;

	GFX_THROW_INFO(device->CreateBuffer(&bufferDesc2, nullptr, m_vsPassConstantsBuffer.ReleaseAndGetAddressOf()));
}

void Scene::Update(const Timer& timer)
{
	auto context = m_deviceResources->D3DDeviceContext();

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

	// Update the constant buffer with the new matrix values
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	GFX_THROW_INFO(context->Map(m_vsPassConstantsBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_passConstants, sizeof(PassConstants));
	GFX_THROW_INFO_ONLY(context->Unmap(m_vsPassConstantsBuffer.Get(), 0));


	// Update all render objects
	std::vector<RenderObject>& objects = std::get<2>(m_configAndObjectList);
	for (unsigned int iii = 0; iii < objects.size(); ++iii)
	{
		objects[iii].Update(timer);
	}
}

void Scene::Render()
{
	EG_ASSERT(m_currentCamera < m_cameras.size(), "The selected camera index must be within the bounds of the m_cameras vector");

	auto context = m_deviceResources->D3DDeviceContext();

	// Set all constant buffers - Including buffers that are constant per rendering pass as well
	// as buffers that will be updated per object
	ID3D11Buffer* const buffers[2] = { m_vsModelBuffer.Get(), m_vsPassConstantsBuffer.Get() };
	GFX_THROW_INFO_ONLY(context->VSSetConstantBuffers(0u, 2u, buffers));

	// Apply the pipeline config for each list of render objects, render each object, then move onto the next config
	std::unique_ptr<PipelineConfig>& config = std::get<0>(m_configAndObjectList);
	config->ApplyConfig();

	std::unique_ptr<MeshSet>& ms = std::get<1>(m_configAndObjectList);
	ms->BindToIA();

	std::vector<RenderObject>& objects = std::get<2>(m_configAndObjectList);
	for (unsigned int iii = 0; iii < objects.size(); ++iii)
	{
		objects[iii].Render();
	}
}