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
	RenderObject(const DirectX::XMFLOAT3& scaling, float* translation);
	
	DirectX::XMFLOAT4X4 WorldMatrix() const noexcept;

private:
	DirectX::XMFLOAT3 m_scaling;
	float* m_translation; // Hold a pointer to the translation data which should be managed elsewhere
};

class RenderObjectList
{
public:
	RenderObjectList(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh);
	
	
	void Update(const Evergreen::Timer& timer);
	void Render() const;

	void AddRenderObject(const DirectX::XMFLOAT3& scaling, float* translation);

private:

	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	MeshInstance m_mesh;

	std::vector<RenderObject> m_renderObjects;
	std::vector<DirectX::XMFLOAT4X4> m_worldMatrices;
};