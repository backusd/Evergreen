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

	//void SetScaling(float x, float y, float z) noexcept;
	//void SetScaling(float xyz) noexcept;
	//void SetTranslation(float x, float y, float z) noexcept;

private:
	//inline void UpdateWorldMatrix();
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	MeshInstance m_mesh;
	//Material m_material;

	//DirectX::XMFLOAT3 m_scaling;
	// DirectX::XMFLOAT3 m_rotation; // Omit a rotation right now because it isn't necessary and you have to specify what axis to rotate on which adds unnecessary complexity right now
	//DirectX::XMFLOAT3 m_translation;

	//DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();

	std::vector<DirectX::XMFLOAT4X4> m_worldMatrices;
};