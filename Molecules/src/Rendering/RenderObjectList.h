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
	RenderObject(const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3* translation, unsigned int materialIndex);
	
	DirectX::XMFLOAT4X4 WorldMatrix() const noexcept;
	unsigned int MaterialIndex() const noexcept { return m_materialIndex; }

private:
	DirectX::XMFLOAT3 m_scaling;
	const DirectX::XMFLOAT3* m_translation; // Hold a pointer to the translation data which should be managed elsewhere

	unsigned int m_materialIndex;
};

class RenderObjectList
{
public:
	RenderObjectList(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh);
	
	
	void Update(const Evergreen::Timer& timer);
	void Render() const;

	void AddRenderObject(const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3* translation, unsigned int materialIndex);

	void SetBufferUpdateCallback(std::function<void(const RenderObjectList*, size_t, size_t)> fn) noexcept { m_bufferUpdateFn = fn; }


	ND inline std::shared_ptr<Evergreen::DeviceResources> GetDeviceResources() const noexcept { return m_deviceResources; }
	ND inline const std::vector<DirectX::XMFLOAT4X4>& GetWorldMatrices() const noexcept { return m_worldMatrices; }
	ND inline const std::vector<unsigned int>& GetMaterialIndices() const noexcept { return m_materialIndices; }
	ND inline Microsoft::WRL::ComPtr<ID3D11Buffer> GetInstanceBuffer() const noexcept { return m_instanceBuffer; }

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	MeshInstance m_mesh;

	// Right now, each instance just requires an index into the materials array
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;

	std::function<void(const RenderObjectList*, size_t, size_t)> m_bufferUpdateFn = [](const RenderObjectList*, size_t, size_t) {};


	std::vector<RenderObject> m_renderObjects;
	std::vector<DirectX::XMFLOAT4X4> m_worldMatrices;
	std::vector<unsigned int> m_materialIndices;
};