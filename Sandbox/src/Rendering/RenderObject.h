#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "Camera.h"
#include "../Utils/MathHelper.h"
#include "MeshSet.h"


class RenderObject
{
public:
	RenderObject(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh);
	void Render(const Camera& camera);
	void SetAspectRatio(float ratio) noexcept { m_aspectRatio = ratio; }

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	MeshInstance m_mesh;

	DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();

	float m_aspectRatio = 0.0f;
};