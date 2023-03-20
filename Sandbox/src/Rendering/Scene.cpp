#include "Scene.h"

using namespace Evergreen;
using namespace DirectX;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources),
	m_currentCamera(0u)
{
	m_cameras.emplace_back();

	std::unique_ptr<PipelineConfig> config = std::make_unique<PipelineConfig>(m_deviceResources);

	std::unique_ptr<MeshSet> ms = std::make_unique<MeshSet>(m_deviceResources);
	std::vector<Vertex> vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Magenta) })
	};
	std::vector<std::uint16_t> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	MeshInstance mi = ms->AddMesh(vertices, indices);
	ms->Finalize();


	std::vector<RenderObject> objects;
	objects.emplace_back(deviceResources, mi);

	m_configAndObjectList = std::make_tuple(std::move(config), std::move(ms), objects);
}

void Scene::SetAspectRatio(float ratio) noexcept 
{	
	std::vector<RenderObject>& objects = std::get<2>(m_configAndObjectList);

	for (unsigned int iii = 0; iii < objects.size(); ++iii)
	{
		objects[iii].SetAspectRatio(ratio);
	}
}

void Scene::Render()
{
	EG_ASSERT(m_currentCamera < m_cameras.size(), "The selected camera index must be within the bounds of the m_cameras vector");

	// Apply the pipeline config for each list of render objects, render each object, then move onto the next config
	
	std::unique_ptr<PipelineConfig>& config = std::get<0>(m_configAndObjectList);
	config->ApplyConfig();

	std::unique_ptr<MeshSet>& ms = std::get<1>(m_configAndObjectList);
	ms->BindToIA();

	std::vector<RenderObject>& objects = std::get<2>(m_configAndObjectList);
	for (unsigned int iii = 0; iii < objects.size(); ++iii)
	{
		objects[iii].Render(m_cameras[m_currentCamera]);
	}
}