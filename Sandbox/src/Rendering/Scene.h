#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "PipelineConfig.h"
#include "../Utils/MathHelper.h"
#include "Camera.h"
#include "RenderObject.h"


class Scene
{
public:
	Scene(std::shared_ptr<Evergreen::DeviceResources> deviceResources);
	void Render();
	void SetAspectRatio(float ratio) noexcept;

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	std::vector<PipelineConfig> m_pipelineConfigs;
	std::vector<std::vector<RenderObject>> m_renderObjectLists;

	std::vector<Camera> m_cameras;
	unsigned int m_currentCamera;
};