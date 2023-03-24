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

	void SetScaling(float x, float y, float z) noexcept;
	void SetScaling(float xyz) noexcept;
	void SetTranslation(float x, float y, float z) noexcept;

private:
	inline void UpdateWorldMatrix();
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	MeshInstance m_mesh;
	Material m_material;

	DirectX::XMFLOAT3 m_scaling;
	// DirectX::XMFLOAT3 m_rotation; // Omit a rotation right now because it isn't necessary and you have to specify what axis to rotate on which adds unnecessary complexity right now
	DirectX::XMFLOAT3 m_translation;

	DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
};