#include "MeshSet.h"

using namespace Evergreen;

MeshSet::MeshSet(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources),
	m_finalized(false),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr)
{
}

MeshInstance MeshSet::AddMesh(const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices)
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");
	EG_ASSERT(vertices.size() > 0, "No vertices to add");
	EG_ASSERT(indices.size() > 0, "No indices to add");

	MeshInstance mi;
	mi.IndexCount = indices.size();
	mi.StartIndexLocation = m_indices.size();
	mi.BaseVertexLocation = m_vertices.size();

	m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
	m_indices.insert(m_indices.end(), indices.begin(), indices.end());

	return mi;
}
void MeshSet::Finalize()
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");

	auto device = m_deviceResources->D3DDevice();

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = m_vertices.size() * sizeof(Vertex); // Size of buffer in bytes
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = m_vertices.data();
	GFX_THROW_INFO(device->CreateBuffer(&bd, &sd, m_vertexBuffer.ReleaseAndGetAddressOf()))

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = m_indices.size() * sizeof(std::uint16_t);
	ibd.StructureByteStride = sizeof(std::uint16_t);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = m_indices.data();
	GFX_THROW_INFO(device->CreateBuffer(&ibd, &isd, m_indexBuffer.ReleaseAndGetAddressOf()))

	m_finalized = true;
}

void MeshSet::BindToIA() const
{
	EG_ASSERT(m_finalized, "The MeshSet has not been finalized");

	auto context = m_deviceResources->D3DDeviceContext();

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	GFX_THROW_INFO_ONLY(context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset))

	GFX_THROW_INFO_ONLY(context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u))
}