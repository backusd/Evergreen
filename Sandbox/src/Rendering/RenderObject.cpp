#include "RenderObject.h"
#include "Structs.h"

using namespace Evergreen;
using namespace DirectX;

using Microsoft::WRL::ComPtr;

RenderObject::RenderObject(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh) :
	m_deviceResources(deviceResources),
	m_mesh(mesh)
{
}

void RenderObject::Update(const Timer& timer)
{
}

void RenderObject::Render() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	ComPtr<ID3D11Buffer> modelBuffer;
	GFX_THROW_INFO_ONLY(context->VSGetConstantBuffers(0, 1, modelBuffer.ReleaseAndGetAddressOf()));
	GFX_THROW_INFO(context->Map(modelBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
	memcpy(ms.pData, &m_world, sizeof(ObjectConstants));
	GFX_THROW_INFO_ONLY(context->Unmap(modelBuffer.Get(), 0));

	GFX_THROW_INFO_ONLY(context->DrawIndexed(m_mesh.IndexCount, m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation));
}