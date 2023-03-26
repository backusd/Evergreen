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
	RenderObject(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh);
	void Update(const Evergreen::Timer& timer);
	void Render() const;

	void AddObject(const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3& translation);

private:

	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	MeshInstance m_mesh;

	std::vector<DirectX::XMFLOAT4X4> m_worldMatrices;
};