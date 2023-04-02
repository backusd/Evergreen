#include "RenderObjectList.h"
#include "Structs.h"

using namespace Evergreen;
using namespace DirectX;

using Microsoft::WRL::ComPtr;

RenderObjectList::RenderObjectList(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh) :
	m_deviceResources(deviceResources),
	m_mesh(mesh)
{
	EG_ASSERT(deviceResources != nullptr, "No device resources");

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

void RenderObjectList::Update(const Timer& timer) noexcept
{
	EG_ASSERT(m_worldMatrices.size() == m_renderObjects.size(), "Number of world matrices and render objects should match");

	// Re-compute all world matrices every frame because their positions will be changing
	for (unsigned int iii = 0; iii < m_renderObjects.size(); ++iii)
	{
		m_worldMatrices[iii] = m_renderObjects[iii].WorldMatrix();
	}
}

void RenderObjectList::Render() const
{
	EG_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_ASSERT(m_worldMatrices.size() == m_renderObjects.size(), "Number of world matrices and render objects should match");

	auto context = m_deviceResources->D3DDeviceContext();
	
	// Loop over the world matrices and draw up to MAX_INSTANCES at a time
	size_t endIndex = 0; 
	for (size_t startIndex = 0; startIndex < m_worldMatrices.size(); startIndex += MAX_INSTANCES)
	{
		endIndex = std::min(startIndex + MAX_INSTANCES, m_worldMatrices.size()) - 1;

		// Need to assign lambda that will update pipeline constant buffers 
		m_bufferUpdateFn(this, startIndex, endIndex);

		GFX_THROW_INFO_ONLY( 
			context->DrawIndexedInstanced(m_mesh.IndexCount, static_cast<UINT>(endIndex - startIndex + 1), m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation, 0u);
		); 
	}
}