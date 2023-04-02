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
	RenderObject(const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3* translation, unsigned int materialIndex) :
		m_scaling(scaling),
		m_translation(translation),
		m_materialIndex(materialIndex)
	{}
	// Must implement copy constructor because it is required when stored in std::vector. See https://stackoverflow.com/questions/40457302/c-vector-emplace-back-calls-copy-constructor
	RenderObject(const RenderObject&) noexcept = default;
	~RenderObject() noexcept {};

	ND inline DirectX::XMFLOAT4X4 WorldMatrix() const noexcept
	{
		using namespace DirectX;

		DirectX::XMFLOAT4X4 world;
		XMStoreFloat4x4(&world,
			XMMatrixTranspose(
				XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z) *
				XMMatrixTranslation(m_translation->x, m_translation->y, m_translation->z)
			)
		);
		return world;
	}
	ND inline unsigned int MaterialIndex() const noexcept 
	{ 
		return m_materialIndex; 
	}

private:
	DirectX::XMFLOAT3			m_scaling;
	const DirectX::XMFLOAT3*	m_translation; // Hold a pointer to the translation data which should be managed elsewhere
	unsigned int				m_materialIndex;
};

// -----------------------------------------------------------------------------------

class RenderObjectList
{
public:
	RenderObjectList(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh);
	// Must implement copy constructor because it is required when stored in std::vector. See https://stackoverflow.com/questions/40457302/c-vector-emplace-back-calls-copy-constructor
	RenderObjectList(const RenderObjectList&) noexcept = default;
	
	void Update(const Evergreen::Timer& timer) noexcept;
	void Render() const;

	inline void AddRenderObject(const DirectX::XMFLOAT3& scaling, const DirectX::XMFLOAT3* translation, unsigned int materialIndex)
	{
		m_renderObjects.emplace_back(scaling, translation, materialIndex);
		m_worldMatrices.push_back(m_renderObjects.back().WorldMatrix());
		m_materialIndices.push_back(materialIndex);
	}
	inline void SetBufferUpdateCallback(std::function<void(const RenderObjectList*, size_t, size_t)> fn) noexcept 
	{ 
		m_bufferUpdateFn = fn; 
	}

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