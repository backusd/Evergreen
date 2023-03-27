#include "RenderObjectList.h"
#include "Structs.h"

using namespace Evergreen;
using namespace DirectX;

using Microsoft::WRL::ComPtr;

RenderObject::RenderObject(const XMFLOAT3& scaling, XMFLOAT3* translation, unsigned int materialIndex) :
	m_scaling(scaling),
	m_translation(translation),
	m_materialIndex(materialIndex)
{}

DirectX::XMFLOAT4X4 RenderObject::WorldMatrix() const noexcept
{
	DirectX::XMFLOAT4X4 world;
	XMStoreFloat4x4(&world,
		XMMatrixTranspose(
			XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z) *
			XMMatrixTranslation(m_translation->x, m_translation->y, m_translation->z)
		)
	);
	return world;
}

// -----------------------------------------------------------------------------------

RenderObjectList::RenderObjectList(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh) :
	m_deviceResources(deviceResources),
	m_mesh(mesh)
{
	auto device = m_deviceResources->D3DDevice();

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0u;
	bd.ByteWidth = static_cast<UINT>(MAX_INSTANCES * sizeof(unsigned int)); // Size of buffer in bytes
	bd.StructureByteStride = sizeof(unsigned int);

	GFX_THROW_INFO(device->CreateBuffer(&bd, nullptr, m_instanceBuffer.ReleaseAndGetAddressOf()));

}
void RenderObjectList::AddRenderObject(const DirectX::XMFLOAT3& scaling, XMFLOAT3* translation, unsigned int materialIndex)
{
	m_renderObjects.emplace_back(scaling, translation, materialIndex);
	m_worldMatrices.push_back(m_renderObjects.back().WorldMatrix());
	m_materialIndices.push_back(materialIndex);
}

void RenderObjectList::Update(const Timer& timer)
{
	EG_CORE_ASSERT(m_worldMatrices.size() == m_renderObjects.size(), "Number of world matrices and render objects should match");

	// Re-compute all world matrices every frame because their positions will be changing
	for (unsigned int iii = 0; iii < m_renderObjects.size(); ++iii)
	{
		m_worldMatrices[iii] = m_renderObjects[iii].WorldMatrix();
	}
}

void RenderObjectList::Render() const
{
	EG_CORE_ASSERT(m_worldMatrices.size() < MAX_INSTANCES, "too many objects");

	auto context = m_deviceResources->D3DDeviceContext();

	D3D11_MAPPED_SUBRESOURCE ms;

	// Update the World buffers at VS slot 1 ------------------------------------------------------
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ComPtr<ID3D11Buffer> buffer = nullptr;

	GFX_THROW_INFO_ONLY(context->VSGetConstantBuffers(1, 1, buffer.ReleaseAndGetAddressOf()));
	GFX_THROW_INFO(context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, m_worldMatrices.data(), sizeof(XMFLOAT4X4) * m_worldMatrices.size());
	GFX_THROW_INFO_ONLY(context->Unmap(buffer.Get(), 0));

	// --------------------------------------------------------------------------------------------
	// Update the instance buffer and then bind it to the IA

	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	GFX_THROW_INFO(context->Map(m_instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, m_materialIndices.data(), sizeof(unsigned int) * m_materialIndices.size());
	GFX_THROW_INFO_ONLY(context->Unmap(m_instanceBuffer.Get(), 0));

	UINT strides[1] = { sizeof(unsigned int) };
	UINT offsets[1] = { 0u };
	ID3D11Buffer* vertInstBuffers[1] = { m_instanceBuffer.Get() };
	GFX_THROW_INFO_ONLY(context->IASetVertexBuffers(1u, 1u, vertInstBuffers, strides, offsets));

	// ---------------------------------------------------------------------------------------------

	//GFX_THROW_INFO_ONLY(context->DrawIndexed(m_mesh.IndexCount, m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation));

	GFX_THROW_INFO_ONLY(
		context->DrawIndexedInstanced(m_mesh.IndexCount, static_cast<UINT>(m_worldMatrices.size()), m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation, 0u);
	);
}