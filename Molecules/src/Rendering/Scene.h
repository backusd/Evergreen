#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "PipelineConfig.h"
#include "../Utils/MathHelper.h"
#include "Camera.h"
#include "RenderObjectList.h"
#include "MeshSet.h"
#include "Structs.h"
#include "../Simulation/Simulation.h"


class Scene
{
	using PipelineConfigAndObjectList = std::tuple<std::unique_ptr<PipelineConfig>, std::unique_ptr<MeshSet>, std::vector<RenderObjectList>>;

public:
	Scene(std::shared_ptr<Evergreen::DeviceResources> deviceResources, Simulation* simulation);
	void Update(const Evergreen::Timer& timer);
	void Render();
	void SetAspectRatio(float ratio) noexcept { m_aspectRatio = ratio; }

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	Simulation* m_simulation;

	std::vector<PipelineConfigAndObjectList> m_configsAndObjectLists;

	// Pass Constants that will be updated/bound only once per pass
	// NOTE: the ConstantBuffer is a shared_ptr so that it can be shared with EVERY PipelineConfig
	PassConstants m_passConstants;
	std::vector<std::shared_ptr<ConstantBuffer>> m_vsPerPassConstantsBuffers;
	std::vector<std::shared_ptr<ConstantBuffer>> m_psPerPassConstantsBuffers;

	float m_aspectRatio;

	std::vector<Camera> m_cameras;
	unsigned int m_currentCamera;
};