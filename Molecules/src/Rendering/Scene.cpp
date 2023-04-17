#include "Scene.h"
#include "../Utils/JSONHelper.h"

using namespace Evergreen;
using namespace DirectX;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources, Simulation* simulation, Viewport* viewport) :
	m_deviceResources(deviceResources),
	m_simulation(simulation),
	m_viewport(viewport)
{
	EG_ASSERT(viewport != nullptr, "Viewport cannot be nullptr");

	// Register a callback with the viewport so that when the viewport size changes, we can update the camera's projection matrix
	m_viewport->SetOnSizeChangedCallback([this](float width, float height)
		{
			this->GetCamera()->UpdateProjectionMatrix();
		}
	);

	m_camera = std::make_unique<Camera>(viewport);

	m_materials = std::make_unique<MaterialsArray>();
	CreateMaterials();

	CreateMainPipelineConfig();
	CreateBoxPipelineConfig();
}
void Scene::CreateMaterials()
{
	const std::string materialsFile = "src/json/materials/materials.json";

	json materialData = LoadJSONFile(materialsFile);
	if (!materialData.contains("materials"))
	{
		EG_ERROR("{}:{} - json data in file '{}' does not contain a 'materials' key. Not able to load any material data. Falling back on default values.", __FILE__, __LINE__, materialsFile);
		LoadDefaultMaterials();
		return;
	}

	if (!materialData["materials"].is_array())
	{
		EG_ERROR("{}:{} - json data in file '{}' -> The value for the 'materials' key is not an array. Not able to load any material data. Falling back on default values.", __FILE__, __LINE__, materialsFile);
		LoadDefaultMaterials();
		return;
	}

	if (materialData["materials"].size() != 10)
	{
		EG_ERROR("{}:{} - json data in file '{}' -> The 'materials' array value is expected to have exactly 10 values, but has {}. Not able to load any material data. Falling back on default values.", __FILE__, __LINE__, materialsFile, materialData["materials"].size());
		LoadDefaultMaterials();
		return;
	}

	std::string elementName = "";
	unsigned int materialIndex = 0;
	XMFLOAT4 diffuseAlbedo = {};
	XMFLOAT3 fresnel = {};
	float shininess = 0.0f;

	for (auto& material : materialData["materials"])
	{
		// Element
		if (!material.contains("Element"))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> An entry in the 'materials' array does not have the key 'Element'. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (!material["Element"].is_string())
		{
			EG_ERROR("{}:{} - json data in file '{}' -> An entry in the 'materials' array has the key 'Element', but the value is not a string. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		elementName = material["Element"].get<std::string>();

		// Material Index
		if (!material.contains("MaterialIndex"))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array does not have the key 'MaterialIndex'. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (!material["MaterialIndex"].is_number_unsigned())
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the key 'MaterialIndex', but the value is not an unsigned int. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		materialIndex = material["MaterialIndex"].get<unsigned int>();

		// Diffuse Albedo
		if (!material.contains("DiffuseAlbedo"))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array does not have the key 'DiffuseAlbedo'. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (!material["DiffuseAlbedo"].is_array())
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the key 'DiffuseAlbedo', but the value is not an array. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (material["DiffuseAlbedo"].size() != 4)
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the 'DiffuseAlbedo' array and the expected size is 4, but the actual size is {}. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material["DiffuseAlbedo"].size(), material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (!(material["DiffuseAlbedo"][0].is_number_float() && material["DiffuseAlbedo"][1].is_number_float() && material["DiffuseAlbedo"][2].is_number_float() && material["DiffuseAlbedo"][3].is_number_float()))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the 'DiffuseAlbedo' array, but not all four values were floats. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		diffuseAlbedo.x = material["DiffuseAlbedo"][0].get<float>();
		diffuseAlbedo.y = material["DiffuseAlbedo"][1].get<float>();
		diffuseAlbedo.z = material["DiffuseAlbedo"][2].get<float>();
		diffuseAlbedo.w = material["DiffuseAlbedo"][3].get<float>();
		if (!(diffuseAlbedo.x >= 0.0f && diffuseAlbedo.x <= 1.0f &&
			  diffuseAlbedo.y >= 0.0f && diffuseAlbedo.y <= 1.0f &&
			  diffuseAlbedo.z >= 0.0f && diffuseAlbedo.z <= 1.0f &&
			  diffuseAlbedo.w >= 0.0f && diffuseAlbedo.w <= 1.0f))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the 'DiffuseAlbedo' array, but not all four values were in the range [0, 1]. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}

		// FresnelR0
		if (!material.contains("FresnelR0"))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array does not have the key 'FresnelR0'. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (!material["FresnelR0"].is_array())
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the key 'FresnelR0', but the value is not an array. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (material["FresnelR0"].size() != 3)
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the 'FresnelR0' array and the expected size is 3, but the actual size is {}. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material["FresnelR0"].size(), material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (!(material["FresnelR0"][0].is_number_float() && material["FresnelR0"][1].is_number_float() && material["FresnelR0"][2].is_number_float()))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the 'FresnelR0' array, but not all three values were floats. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		fresnel.x = material["FresnelR0"][0].get<float>();
		fresnel.y = material["FresnelR0"][1].get<float>();
		fresnel.z = material["FresnelR0"][2].get<float>();
		if (!(fresnel.x >= 0.0f && fresnel.x <= 1.0f &&
			fresnel.y >= 0.0f && fresnel.y <= 1.0f &&
			fresnel.z >= 0.0f && fresnel.z <= 1.0f))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the 'FresnelR0' array, but not all thrre values were in the range [0, 1]. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}

		// Shininess
		if (!material.contains("Shininess"))
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array does not have the key 'Shininess'. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		if (!material["Shininess"].is_number_float())
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the key 'Shininess', but the value is not a float. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}
		shininess = material["Shininess"].get<float>();
		if (shininess < 0.0f || shininess > 1.0f)
		{
			EG_ERROR("{}:{} - json data in file '{}' -> The '{}' entry in the 'materials' array has the key 'Shininess', but the value is not in the range [0, 1]. Not able to load any material data. Falling back on default values.\nInvalid entry: {}", __FILE__, __LINE__, materialsFile, elementName, material.dump(4));
			LoadDefaultMaterials();
			return;
		}

		// Load the data
		m_materials->materials[materialIndex].DiffuseAlbedo = diffuseAlbedo;
		m_materials->materials[materialIndex].FresnelR0 = fresnel;
		m_materials->materials[materialIndex].Shininess = shininess;
	}
}
void Scene::LoadDefaultMaterials()
{
	// Load default values
	for (unsigned int iii = 0; iii < 10; ++iii)
	{
		m_materials->materials[iii].FresnelR0 = { 0.1f, 0.1f, 0.1f };
		m_materials->materials[iii].Shininess = 0.875f;
	}
	m_materials->materials[0].DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials->materials[1].DiffuseAlbedo = XMFLOAT4(1.0f, 0.6f, 0.6f, 1.0f);
	m_materials->materials[2].DiffuseAlbedo = XMFLOAT4(0.4f, 0.7f, 0.7f, 1.0f);
	m_materials->materials[3].DiffuseAlbedo = XMFLOAT4(0.5f, 0.6f, 0.1f, 1.0f);
	m_materials->materials[4].DiffuseAlbedo = XMFLOAT4(0.6f, 0.5f, 0.9f, 1.0f);
	m_materials->materials[5].DiffuseAlbedo = XMFLOAT4(0.7f, 0.4f, 0.2f, 1.0f);
	m_materials->materials[6].DiffuseAlbedo = XMFLOAT4(0.8f, 0.3f, 0.3f, 1.0f);
	m_materials->materials[7].DiffuseAlbedo = XMFLOAT4(0.9f, 0.2f, 0.9f, 1.0f);
	m_materials->materials[8].DiffuseAlbedo = XMFLOAT4(0.2f, 0.1f, 0.5f, 1.0f);
	m_materials->materials[9].DiffuseAlbedo = XMFLOAT4(0.3f, 0.0f, 0.3f, 1.0f);
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
	EG_ASSERT(m_materials != nullptr, "Materials have not been created");
	m_materialsBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	m_materialsBuffer->CreateBuffer<MaterialsArray>(D3D11_USAGE_DEFAULT, 0u, 0u, 0u, m_materials.get());

	m_psPerPassConstantsBuffers.push_back(m_materialsBuffer);
	psCBA->AddBuffer(m_materialsBuffer);

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

	objectLists.back().SetBufferUpdateCallback([](const RenderObjectList* renderObjectList, size_t startIndex, size_t endIndex)
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
			// Only copy the elements between the start and end indices
			memcpy(ms.pData, &worldMatrices.data()[startIndex], sizeof(XMFLOAT4X4) * (endIndex - startIndex + 1));
			GFX_THROW_INFO_ONLY(context->Unmap(buffer.Get(), 0)); 

			// --------------------------------------------------------------------------------------------
			// Update the instance buffer and then bind it to the IA

			Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer = renderObjectList->GetInstanceBuffer();

			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE)); 

			GFX_THROW_INFO(context->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)); 
			// Only copy the elements between the start and end indices
			memcpy(ms.pData, &materialIndices.data()[startIndex], sizeof(unsigned int) * (endIndex - startIndex + 1));
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
	objectLists.back().SetBufferUpdateCallback([this](const RenderObjectList* renderObjectList, size_t startIndex, size_t endIndex)
		{
			using Microsoft::WRL::ComPtr;
			using namespace DirectX;

			Camera* camera = this->GetCamera();
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

void Scene::Update(const Timer& timer)
{
	auto context = m_deviceResources->D3DDeviceContext();

	// Update the Camera ---------------------------------------------------------------------
	m_camera->Update(timer);

	// Update Pass Constants -----------------------------------------------------------------

	XMMATRIX view = m_camera->ViewMatrix();
	XMMATRIX proj = m_camera->ProjectionMatrix();

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
	m_passConstants.EyePosW = m_camera->Position();
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
	m_camera->OnChar(e.GetKeyCode());
}
void Scene::OnKeyPressed(KeyPressedEvent& e)
{
	m_camera->OnKeyPressed(e.GetKeyCode());
}
void Scene::OnKeyReleased(KeyReleasedEvent& e)
{
	m_camera->OnKeyReleased(e.GetKeyCode());
}
void Scene::OnMouseEntered(MouseMoveEvent& e)
{

}
void Scene::OnMouseExited(MouseMoveEvent& e)
{

}
void Scene::OnMouseMoved(MouseMoveEvent& e)
{
	m_camera->OnMouseMove(e.GetX(), e.GetY());
}
void Scene::OnMouseScrolledVertical(MouseScrolledEvent& e)
{
	m_camera->OnMouseScrolledVertical(e.GetX(), e.GetY(), e.GetScrollDelta());
}
void Scene::OnMouseScrolledHorizontal(MouseScrolledEvent& e)
{
	m_camera->OnMouseScrolledHorizontal(e.GetX(), e.GetY(), e.GetScrollDelta());
}
void Scene::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	switch (e.GetMouseButton())
	{
	case MOUSE_BUTTON::EG_LBUTTON: 
		m_camera->OnLButtonPressed(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_RBUTTON:
		m_camera->OnRButtonPressed(e.GetX(), e.GetY());
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
		m_camera->OnLButtonReleased(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_RBUTTON:
		m_camera->OnRButtonReleased(e.GetX(), e.GetY());
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
		m_camera->OnLButtonDoubleClick(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_RBUTTON:
		m_camera->OnRButtonDoubleClick(e.GetX(), e.GetY());
		break;
	case MOUSE_BUTTON::EG_MBUTTON: break;
	case MOUSE_BUTTON::EG_XBUTTON1: break;
	case MOUSE_BUTTON::EG_XBUTTON2: break;
	}
}