#pragma once
#include "pch.h"
#include "PipelineConfig.h"
#include "../Utils/MathHelper.h"
#include <Evergreen.h>

class Scene
{
public:
	Scene(std::shared_ptr<Evergreen::DeviceResources> deviceResources);
	void Render();
	void SetAspectRatio(float ratio) noexcept;

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	std::vector<PipelineConfig> m_pipelineConfigs;

	DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();
	float m_theta = 1.5f * DirectX::XM_PI;
	float m_phi = DirectX::XM_PIDIV4;
	float m_radius = 5.0f;

	float m_aspectRatio = 0.0f;
};