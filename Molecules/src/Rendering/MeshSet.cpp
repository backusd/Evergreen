#include "MeshSet.h"

using namespace Evergreen;
using namespace DirectX;

MeshSetBase::MeshSetBase(std::shared_ptr<Evergreen::DeviceResources> deviceResources) :
	m_deviceResources(deviceResources),
	m_sizeOfT(0u),
	m_finalized(false),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr)
{
}

void MeshSetBase::BindToIA() const
{
	EG_ASSERT(m_finalized, "The MeshSet has not been finalized");
	EG_ASSERT(m_sizeOfT > 0, "The deriving MeshSet<T> must set this equal to sizeof(T)");

	auto context = m_deviceResources->D3DDeviceContext();

	// NOTE: Always bind the vertex buffer to slot #0 on the IA. When doing instanced rendering and a secondary instance
	//       buffer is necessary, we can call IASetVertexBuffers to specifically set it to a slot other than slot #0
	UINT stride[1] = { m_sizeOfT };
	UINT offset[1] = { 0u };
	ID3D11Buffer* vertexBuffer[1] = { m_vertexBuffer.Get() };
	GFX_THROW_INFO_ONLY(context->IASetVertexBuffers(0u, 1u, vertexBuffer, stride, offset));

	GFX_THROW_INFO_ONLY(context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
}
