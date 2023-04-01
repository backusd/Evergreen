#include "Scene.h"

using namespace Evergreen;
using namespace DirectX;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources, Simulation* simulation, Viewport* viewport) :
	m_deviceResources(deviceResources),
	m_simulation(simulation),
	m_currentCamera(0u),
	m_viewport(viewport)
{
	EG_ASSERT(viewport != nullptr, "Viewport cannot be nullptr");

	m_cameras.emplace_back(viewport);

	CreateMainPipelineConfig();
	CreateBoxPipelineConfig();
}
void Scene::CreateMainPipelineConfig()
{
	// Shaders
	std::unique_ptr<PixelShader> ps = std::make_unique<PixelShader>(m_deviceResources, L"PixelShaderInstanced.cso");
	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(m_deviceResources, L"VertexShaderInstanced.cso");

	// Input Layout
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	inputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	inputElements.push_back({ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	// Instance Data ---------------------------------------------
	inputElements.push_back({ "MATERIAL_INDEX", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
	std::unique_ptr<InputLayout> il = std::make_unique<InputLayout>(m_deviceResources, inputElements, vs.get());

	// Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterDesc; // Fill with default values for now
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME; // ;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	std::unique_ptr<RasterizerState> rs = std::make_unique<RasterizerState>(m_deviceResources, rasterDesc);

	// Blend State
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
	std::unique_ptr<BlendState> bs = std::make_unique<BlendState>(m_deviceResources, blendDesc);

	// Depth Stencil State
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
	std::unique_ptr<DepthStencilState> dss = std::make_unique<DepthStencilState>(m_deviceResources, depthStencilDesc);

	// VS Buffers --------------
	std::unique_ptr<ConstantBufferArray> vsCBA = std::make_unique<ConstantBufferArray>(m_deviceResources);
	
	// Buffer #1: PassConstants - Will be updated by Scene once per rendering pass
	std::shared_ptr<ConstantBuffer> vsPassConstantsBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	vsPassConstantsBuffer->CreateBuffer<PassConstants>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	m_vsPerPassConstantsBuffers.push_back(vsPassConstantsBuffer); // The Scene must keep track of this buffer because it is responsible for updating it
	vsCBA->AddBuffer(vsPassConstantsBuffer);

	// Buffer #2: WorldMatrixInstances - Will be updated by each RenderObjectList before each Draw call
	std::shared_ptr<ConstantBuffer> vsWorldMatrixInstancesBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	vsWorldMatrixInstancesBuffer->CreateBuffer<WorldMatrixInstances>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	vsCBA->AddBuffer(vsWorldMatrixInstancesBuffer);

	// PS Buffers --------------
	std::unique_ptr<ConstantBufferArray> psCBA = std::make_unique<ConstantBufferArray>(m_deviceResources);

	// Buffer #1: PassConstants - Will be updated by Scene once per rendering pass
	std::shared_ptr<ConstantBuffer> psPassConstantsBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	psPassConstantsBuffer->CreateBuffer<PassConstants>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	m_psPerPassConstantsBuffers.push_back(psPassConstantsBuffer);
	psCBA->AddBuffer(psPassConstantsBuffer);

	// Buffer #2: MaterialsArray - Buffer with all materials that will not ever be updated
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
	psCBA->AddBuffer(psMaterialsArrayBuffer);


	// Pipeline Configuration 
	std::unique_ptr<PipelineConfig> config = std::make_unique<PipelineConfig>(m_deviceResources, 
		std::move(vs),
		std::move(ps),
		std::move(il),
		std::move(rs),
		std::move(bs),
		std::move(dss),
		std::move(vsCBA),
		std::move(psCBA)
	);

	// -------------------------------------------------
	// Mesh Set
	std::unique_ptr<MeshSet<Vertex>> ms = std::make_unique<MeshSet<Vertex>>(m_deviceResources);
	ms->SetVertexConversionFunction([](std::vector<GenericVertex> input) -> std::vector<Vertex>
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

	// RenderObjectLists ----------------------------------------------------------------------------
	std::vector<DirectX::XMFLOAT3>& positions = m_simulation->Positions();
	//std::vector<DirectX::XMFLOAT3>& velocities = m_simulation->Velocities(); Not needed right now
	std::vector<Element>& elementTypes = m_simulation->ElementTypes();

	std::vector<RenderObjectList> objectLists;
	objectLists.emplace_back(m_deviceResources, mi);

	float r;
	unsigned int elementType;
	for (unsigned int iii = 0; iii < positions.size(); ++iii)
	{
		elementType = static_cast<int>(elementTypes[iii]);
		r = AtomicRadii[elementType];
		objectLists.back().AddRenderObject({ r, r, r }, &positions.data()[iii], elementType - 1); // must subtract one because Hydrogen is 1, but its material is at index 0, etc.
	}

	objectLists.back().SetBufferUpdateCallback([](const RenderObjectList* renderObjectList)
		{
			using Microsoft::WRL::ComPtr;

			auto context = renderObjectList->GetDeviceResources()->D3DDeviceContext();

			const std::vector<DirectX::XMFLOAT4X4>& worldMatrices = renderObjectList->GetWorldMatrices();
			const std::vector<unsigned int>& materialIndices = renderObjectList->GetMaterialIndices();

			D3D11_MAPPED_SUBRESOURCE ms; 

			// Update the World buffers at VS slot 1 ------------------------------------------------------
			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE)); 
			ComPtr<ID3D11Buffer> buffer = nullptr; 

			GFX_THROW_INFO_ONLY(context->VSGetConstantBuffers(1, 1, buffer.ReleaseAndGetAddressOf())); 
			GFX_THROW_INFO(context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)); 
			memcpy(ms.pData, worldMatrices.data(), sizeof(XMFLOAT4X4) * worldMatrices.size());  
			GFX_THROW_INFO_ONLY(context->Unmap(buffer.Get(), 0)); 

			// --------------------------------------------------------------------------------------------
			// Update the instance buffer and then bind it to the IA

			Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer = renderObjectList->GetInstanceBuffer();

			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE)); 

			GFX_THROW_INFO(context->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)); 
			memcpy(ms.pData, materialIndices.data(), sizeof(unsigned int) * materialIndices.size()); 
			GFX_THROW_INFO_ONLY(context->Unmap(instanceBuffer.Get(), 0)); 

			UINT strides[1] = { sizeof(unsigned int) }; 
			UINT offsets[1] = { 0u }; 
			ID3D11Buffer* vertInstBuffers[1] = { instanceBuffer.Get() }; 
			GFX_THROW_INFO_ONLY(context->IASetVertexBuffers(1u, 1u, vertInstBuffers, strides, offsets)); 
		}
	);

	m_configsAndObjectLists.push_back(std::make_tuple(std::move(config), std::move(ms), objectLists));
}
void Scene::CreateBoxPipelineConfig()
{
	// Shaders
	std::unique_ptr<PixelShader> ps = std::make_unique<PixelShader>(m_deviceResources, L"BoxPixelShader.cso");
	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(m_deviceResources, L"BoxVertexShader.cso");

	// Input Layout
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	inputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	std::unique_ptr<InputLayout> il = std::make_unique<InputLayout>(m_deviceResources, inputElements, vs.get());

	// Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterDesc; // Fill with default values for now
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME; // ;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	std::unique_ptr<RasterizerState> rs = std::make_unique<RasterizerState>(m_deviceResources, rasterDesc);

	// Blend State
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
	std::unique_ptr<BlendState> bs = std::make_unique<BlendState>(m_deviceResources, blendDesc);

	// Depth Stencil State
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
	std::unique_ptr<DepthStencilState> dss = std::make_unique<DepthStencilState>(m_deviceResources, depthStencilDesc);

	// VS Buffers --------------
	std::unique_ptr<ConstantBufferArray> vsCBA = std::make_unique<ConstantBufferArray>(m_deviceResources);

	// Buffer #1: WorldViewProjection - Will be updated by Scene once per frame
	std::shared_ptr<ConstantBuffer> vsWorldViewProjectionBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	vsWorldViewProjectionBuffer->CreateBuffer<WorldViewProjectionMatrix>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

	vsCBA->AddBuffer(vsWorldViewProjectionBuffer);

	// Pipeline Configuration 
	std::unique_ptr<PipelineConfig> config = std::make_unique<PipelineConfig>(m_deviceResources,
		std::move(vs),
		std::move(ps),
		std::move(il),
		std::move(rs),
		std::move(bs),
		std::move(dss),
		std::move(vsCBA),
		nullptr
	);
	config->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	// -------------------------------------------------
	// Mesh Set
	float x = 1.0f;
	float y = 1.0f;
	float z = 1.0f;
	std::vector<BoxVertex> vertices(8);
	vertices[0].Position = XMFLOAT3( x,  y,  z);
	vertices[1].Position = XMFLOAT3(-x,  y,  z);
	vertices[2].Position = XMFLOAT3( x, -y,  z);
	vertices[3].Position = XMFLOAT3( x,  y, -z);
	vertices[4].Position = XMFLOAT3(-x, -y,  z);
	vertices[5].Position = XMFLOAT3(-x,  y, -z);
	vertices[6].Position = XMFLOAT3( x, -y, -z);
	vertices[7].Position = XMFLOAT3(-x, -y, -z);

	std::vector<std::uint16_t> indices(24);

	// draw the square with all positive x
	indices[0] = 0;
	indices[1] = 3;
	indices[2] = 3;
	indices[3] = 6;
	indices[4] = 6;
	indices[5] = 2;
	indices[6] = 2;
	indices[7] = 0;

	// draw the square with all negative x
	indices[8] = 1;
	indices[9] = 5;
	indices[10] = 5;
	indices[11] = 7;
	indices[12] = 7;
	indices[13] = 4;
	indices[14] = 4;
	indices[15] = 1;

	// draw the four lines that connect positive x with negative x
	indices[16] = 0;
	indices[17] = 1;
	indices[18] = 3;
	indices[19] = 5;
	indices[20] = 6;
	indices[21] = 7;
	indices[22] = 2;
	indices[23] = 4;

	std::unique_ptr<MeshSet<BoxVertex>> ms = std::make_unique<MeshSet<BoxVertex>>(m_deviceResources);
	MeshInstance mi = ms->AddMesh(vertices, indices); 
	ms->Finalize();

	// RenderObjectList ----------------------------------------------------------------------------

	XMFLOAT3 scaling = m_simulation->BoxScaling();
	const XMFLOAT3* translation = m_simulation->BoxTranslation();

	std::vector<RenderObjectList> objectLists; 
	objectLists.emplace_back(m_deviceResources, mi);
	objectLists.back().AddRenderObject(scaling, translation, 0u);
	objectLists.back().SetBufferUpdateCallback([this](const RenderObjectList* renderObjectList)
		{
			using Microsoft::WRL::ComPtr;
			using namespace DirectX;

			const Camera* camera = this->GetCurrentCamera();
			XMMATRIX viewProj = camera->ViewMatrix() * camera->ProjectionMatrix();

			const std::vector<DirectX::XMFLOAT4X4>& worldMatrices = renderObjectList->GetWorldMatrices();
			EG_ASSERT(worldMatrices.size() == 1, "There should be exactly 1 world matrix for 1 simulation box");

			XMMATRIX worldViewProjection = XMMatrixTranspose(XMLoadFloat4x4(&worldMatrices[0]) * viewProj);

			auto context = renderObjectList->GetDeviceResources()->D3DDeviceContext();
			D3D11_MAPPED_SUBRESOURCE ms; 

			// Update the World-View-Projection buffer at VS slot 0 ------------------------------------------------------
			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE)); 
			ComPtr<ID3D11Buffer> buffer = nullptr;  

			GFX_THROW_INFO_ONLY(context->VSGetConstantBuffers(0, 1, buffer.ReleaseAndGetAddressOf())); 
			GFX_THROW_INFO(context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)); 
			memcpy(ms.pData, &worldViewProjection, sizeof(XMMATRIX)); 
			GFX_THROW_INFO_ONLY(context->Unmap(buffer.Get(), 0));
		}
	);

	m_configsAndObjectLists.push_back(std::make_tuple(std::move(config), std::move(ms), objectLists)); 
}

void Scene::SetAspectRatio(float ratio) noexcept 
{ 
	for (auto& camera : m_cameras)
		camera.SetAspectRatio(ratio);
}

void Scene::Update(const Timer& timer)
{
	auto context = m_deviceResources->D3DDeviceContext();

	// Update the Camera ---------------------------------------------------------------------
	m_cameras[m_currentCamera].Update(timer);

	// Update Pass Constants -----------------------------------------------------------------

	XMMATRIX view = m_cameras[m_currentCamera].ViewMatrix();
	XMMATRIX proj = m_cameras[m_currentCamera].ProjectionMatrix();

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
	m_passConstants.EyePosW = m_cameras[m_currentCamera].Position();
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

		std::unique_ptr<MeshSetBase>& ms = std::get<1>(configAndObjectList);
		ms->BindToIA();

		std::vector<RenderObjectList>& objectLists = std::get<2>(configAndObjectList);
		for (unsigned int iii = 0; iii < objectLists.size(); ++iii)
		{
			objectLists[iii].Render();
		}
	}
}


void Scene::OnChar(CharEvent& e)
{
	m_cameras[m_currentCamera].OnChar(e.GetKeyCode());
}
void Scene::OnKeyPressed(KeyPressedEvent& e)
{
	m_cameras[m_currentCamera].OnKeyPressed(e.GetKeyCode());
}
void Scene::OnKeyReleased(KeyReleasedEvent& e)
{
	m_cameras[m_currentCamera].OnKeyReleased(e.GetKeyCode());
}
void Scene::OnMouseEntered(MouseMoveEvent& e)
{

}
void Scene::OnMouseExited(MouseMoveEvent& e)
{

}
void Scene::OnMouseMoved(MouseMoveEvent& e)
{
	m_cameras[m_currentCamera].OnMouseMove(e.GetX(), e.GetY());
}
void Scene::OnMouseScrolledVertical(MouseScrolledEvent& e)
{
	m_cameras[m_currentCamera].OnMouseScrolledVertical(e.GetX(), e.GetY(), e.GetScrollDelta());
}
void Scene::OnMouseScrolledHorizontal(MouseScrolledEvent& e)
{
	m_cameras[m_currentCamera].OnMouseScrolledHorizontal(e.GetX(), e.GetY(), e.GetScrollDelta());
}
void Scene::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	switch (e.GetMouseButton())
	{
	case MOUSE_BUTTON::EG_LBUTTON: 
		m_cameras[m_currentCamera].OnLButtonPressed(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_RBUTTON:
		m_cameras[m_currentCamera].OnRButtonPressed(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_MBUTTON: break;
	case MOUSE_BUTTON::EG_XBUTTON1: break;
	case MOUSE_BUTTON::EG_XBUTTON2: break;
	}
}
void Scene::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	switch (e.GetMouseButton())
	{
	case MOUSE_BUTTON::EG_LBUTTON:
		m_cameras[m_currentCamera].OnLButtonReleased(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_RBUTTON:
		m_cameras[m_currentCamera].OnRButtonReleased(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_MBUTTON: break;
	case MOUSE_BUTTON::EG_XBUTTON1: break;
	case MOUSE_BUTTON::EG_XBUTTON2: break;
	}
}
void Scene::OnClick(MouseButtonReleasedEvent& e)
{

}
void Scene::OnDoubleClick(MouseButtonDoubleClickEvent& e)
{
	switch (e.GetMouseButton())
	{
	case MOUSE_BUTTON::EG_LBUTTON:
		m_cameras[m_currentCamera].OnLButtonDoubleClick(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_RBUTTON:
		m_cameras[m_currentCamera].OnRButtonDoubleClick(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_MBUTTON: break;
	case MOUSE_BUTTON::EG_XBUTTON1: break;
	case MOUSE_BUTTON::EG_XBUTTON2: break;
	}
}