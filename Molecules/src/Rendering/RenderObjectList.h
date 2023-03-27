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
	RenderObject(const DirectX::XMFLOAT3& scaling, DirectX::XMFLOAT3* translation, unsigned int materialIndex);
	
	DirectX::XMFLOAT4X4 WorldMatrix() const noexcept;
	unsigned int MaterialIndex() const noexcept { return m_materialIndex; }

private:
	DirectX::XMFLOAT3 m_scaling;
	DirectX::XMFLOAT3* m_translation; // Hold a pointer to the translation data which should be managed elsewhere

	unsigned int m_materialIndex;
};

class RenderObjectList
{
public:
	RenderObjectList(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh);
	
	
	void Update(const Evergreen::Timer& timer);
	void Render() const;

	void AddRenderObject(const DirectX::XMFLOAT3& scaling, DirectX::XMFLOAT3* translation, unsigned int materialIndex);

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	MeshInstance m_mesh;

	// Right now, each instance just requires an index into the materials array
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;


	std::vector<RenderObject> m_renderObjects;
	std::vector<DirectX::XMFLOAT4X4> m_worldMatrices;
	std::vector<unsigned int> m_materialIndices;
};