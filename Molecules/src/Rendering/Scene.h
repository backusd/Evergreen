#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "PipelineConfig.h"
#include "../Utils/MathHelper.h"
#include "Camera.h"
#include "RenderObject.h"
#include "MeshSet.h"
#include "Structs.h"


class Scene
{
	using PipelineConfigAndObjectList = std::tuple<std::unique_ptr<PipelineConfig>, std::unique_ptr<MeshSet>, std::vector<RenderObject>>;

public:
	Scene(std::shared_ptr<Evergreen::DeviceResources> deviceResources);
	void Update(const Evergreen::Timer& timer);
	void Render();
	void SetAspectRatio(float ratio) noexcept { m_aspectRatio = ratio; }

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	std::vector<PipelineConfigAndObjectList> m_configsAndObjectLists;

	// Pass Constants that will be updated/bound only once per pass
	// NOTE: the ConstantBuffer is a shared_ptr so that it can be shared with EVERY PipelineConfig
	PassConstants m_passConstants;
	std::shared_ptr<ConstantBuffer> m_vsPassConstantsBuffer;
	std::shared_ptr<ConstantBuffer> m_psPassConstantsBuffer;

	float m_aspectRatio;

	std::vector<Camera> m_cameras;
	unsigned int m_currentCamera;
};