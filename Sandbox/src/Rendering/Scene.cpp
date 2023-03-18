#include "Scene.h"

using namespace Evergreen;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources),
	m_currentCamera(0u)
{
	m_pipelineConfigs.emplace_back(deviceResources);
	m_cameras.emplace_back();

	std::vector<RenderObject> objects;
	objects.emplace_back(deviceResources);
	m_renderObjectLists.push_back(objects);
}

void Scene::SetAspectRatio(float ratio) noexcept 
{	
	for (unsigned int iii = 0; iii < m_renderObjectLists.size(); ++iii)
	{
		for (unsigned int jjj = 0; jjj < m_renderObjectLists[iii].size(); ++jjj)
			m_renderObjectLists[iii][jjj].SetAspectRatio(ratio);
	}
}

void Scene::Render()
{
	EG_ASSERT(m_pipelineConfigs.size() == m_renderObjectLists.size(), "The number of pipeline configs and lists of render objects must match.");
	EG_ASSERT(m_currentCamera < m_cameras.size(), "The selected camera index must be within the bounds of the m_cameras vector");

	// Apply the pipeline config for each list of render objects, render each object, then move onto the next config
	for (unsigned int iii = 0; iii < m_pipelineConfigs.size(); ++iii)
	{
		m_pipelineConfigs[iii].ApplyConfig();

		for (unsigned int jjj = 0; jjj < m_renderObjectLists[iii].size(); ++jjj)
			m_renderObjectLists[iii][jjj].Render(m_cameras[m_currentCamera]);
	}
}