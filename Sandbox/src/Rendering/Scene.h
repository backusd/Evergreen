#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "PipelineConfig.h"
#include "../Utils/MathHelper.h"
#include "Camera.h"
#include "RenderObject.h"
#include "MeshSet.h"


class Scene
{
	using PipelineConfigAndObjectList = std::tuple<std::unique_ptr<PipelineConfig>, std::unique_ptr<MeshSet>, std::vector<RenderObject>>;


public:
	Scene(std::shared_ptr<Evergreen::DeviceResources> deviceResources);
	void Render();
	void SetAspectRatio(float ratio) noexcept;

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	PipelineConfigAndObjectList m_configAndObjectList;

	std::vector<Camera> m_cameras;
	unsigned int m_currentCamera;
};