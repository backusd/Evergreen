#include "Scene.h"

using namespace Evergreen;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources)
{
	m_pipelineConfigs.emplace_back(deviceResources);
}

void Scene::SetAspectRatio(float ratio) noexcept 
{	
	for (auto& config : m_pipelineConfigs)
		config.SetAspectRatio(ratio);
}

void Scene::Render()
{
	for (auto& config : m_pipelineConfigs)
		config.ApplyConfig();
}