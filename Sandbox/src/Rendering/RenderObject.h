#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "Camera.h"
#include "../Utils/MathHelper.h"
#include "MeshSet.h"
#include "Structs.h"


class RenderObject
{
public:
	RenderObject(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh, const Material& material);
	void Update(const Evergreen::Timer& timer);
	void Render() const;

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	MeshInstance m_mesh;
	Material m_material;

	DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
};