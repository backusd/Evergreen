#include "Scene.h"

using namespace Evergreen;
using namespace DirectX;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources, Simulation* simulation, float aspectRatio) :
	m_deviceResources(deviceResources),
	m_simulation(simulation),
	m_currentCamera(0u)
{
	m_cameras.emplace_back(aspectRatio);

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
//	std::unique_ptr<MeshSet> ms = std::make_unique<MeshSet>(m_deviceResources);
//	ms->SetVertexConversionFunction([](std::vector<MeshSet::GeneralVertex> input) -> std::vector<Vertex>
//		{
//			std::vector<Vertex> output(input.size());
//			for (unsigned int iii = 0; iii < input.size(); ++iii)
//			{
//				output[iii].Pos = input[iii].Position;
//				output[iii].Normal = input[iii].Normal;
//			}
//			return output;
//		}
//	);
//	MeshInstance mi = ms->AddGeosphere(1.0f, 3);
//	ms->Finalize();
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
	
}
void Scene::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case KEY_CODE::EG_LEFT_ARROW: m_cameras[m_currentCamera].LeftArrowDown(true); break;
	case KEY_CODE::EG_RIGHT_ARROW: m_cameras[m_currentCamera].RightArrowDown(true); break;
	}
}
void Scene::OnKeyReleased(KeyReleasedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case KEY_CODE::EG_LEFT_ARROW: m_cameras[m_currentCamera].LeftArrowDown(false); break;
	case KEY_CODE::EG_RIGHT_ARROW: m_cameras[m_currentCamera].RightArrowDown(false); break;
	}
}
void Scene::OnMouseEntered(MouseMoveEvent& e)
{

}
void Scene::OnMouseExited(MouseMoveEvent& e)
{

}
void Scene::OnMouseMoved(MouseMoveEvent& e)
{

}
void Scene::OnMouseScrolledVertical(MouseScrolledEvent& e)
{

}
void Scene::OnMouseScrolledHorizontal(MouseScrolledEvent& e)
{

}
void Scene::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{

}
void Scene::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{

}
void Scene::OnClick(MouseButtonReleasedEvent& e)
{

}
void Scene::OnDoubleClick(MouseButtonDoubleClickEvent& e)
{

}