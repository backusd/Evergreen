#include "RenderObject.h"
#include "Structs.h"

using namespace Evergreen;
using namespace DirectX;

using Microsoft::WRL::ComPtr;

RenderObject::RenderObject(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh, const Material& material) :
	m_deviceResources(deviceResources),
	m_mesh(mesh),
	m_material(material),
	m_scaling(1.0f, 1.0f, 1.0f),
	m_translation(0.0f, 0.0f, 0.0f)
{
}

void RenderObject::Update(const Timer& timer)
{
}

void RenderObject::SetScaling(float x, float y, float z) noexcept
{
	m_scaling = XMFLOAT3(x, y, z);
	UpdateWorldMatrix();
}
void RenderObject::SetScaling(float xyz) noexcept
{
	m_scaling = XMFLOAT3(xyz, xyz, xyz);
	UpdateWorldMatrix();
}
void RenderObject::SetTranslation(float x, float y, float z) noexcept
{
	m_translation = XMFLOAT3(x, y, z);
	UpdateWorldMatrix();
}
void RenderObject::UpdateWorldMatrix()
{
	XMStoreFloat4x4(&m_world, 
		XMMatrixTranspose(
			XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z) *
			XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z)
		)
	);
}

void RenderObject::Render() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	D3D11_MAPPED_SUBRESOURCE ms;

	// Update the Material Buffer at VS slot 1
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ComPtr<ID3D11Buffer> buffer = nullptr;

	GFX_THROW_INFO_ONLY(context->VSGetConstantBuffers(1, 1, buffer.ReleaseAndGetAddressOf()));
	GFX_THROW_INFO(context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_material, sizeof(Material));
	GFX_THROW_INFO_ONLY(context->Unmap(buffer.Get(), 0));

	// Update the Model buffer at VS slot 2
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
	buffer = nullptr;

	GFX_THROW_INFO_ONLY(context->VSGetConstantBuffers(2, 1, buffer.ReleaseAndGetAddressOf()));
	GFX_THROW_INFO(context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_world, sizeof(ObjectConstants));
	GFX_THROW_INFO_ONLY(context->Unmap(buffer.Get(), 0));


	// Update the Material Buffer at PS slot 1
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
	buffer = nullptr;

	GFX_THROW_INFO_ONLY(context->PSGetConstantBuffers(1, 1, buffer.ReleaseAndGetAddressOf()));
	GFX_THROW_INFO(context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_material, sizeof(Material));
	GFX_THROW_INFO_ONLY(context->Unmap(buffer.Get(), 0));



	GFX_THROW_INFO_ONLY(context->DrawIndexed(m_mesh.IndexCount, m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation));
}