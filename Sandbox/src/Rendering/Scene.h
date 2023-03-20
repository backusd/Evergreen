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

	PipelineConfigAndObjectList m_configAndObjectList;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vsModelBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vsPassConstantsBuffer;
	PassConstants m_passConstants;

	float m_aspectRatio;

	std::vector<Camera> m_cameras;
	unsigned int m_currentCamera;
};