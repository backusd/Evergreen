#include "MeshSet.h"

using namespace Evergreen;
using namespace DirectX;

MeshSet::MeshSet(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources),
	m_finalized(false),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_conversionFunctionIsSet(false)
{
	// Create a dummy default conversion function
	m_VertexConversionFn = [](std::vector<GeneralVertex> input) -> std::vector<Vertex>
	{
		std::vector<Vertex> output;
		return output;
	};
}

MeshInstance MeshSet::AddMesh(const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices)
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");
	EG_ASSERT(vertices.size() > 0, "No vertices to add");
	EG_ASSERT(indices.size() > 0, "No indices to add");

	MeshInstance mi;
	mi.IndexCount = static_cast<UINT>(indices.size());
	mi.StartIndexLocation = static_cast<UINT>(m_indices.size());
	mi.BaseVertexLocation = static_cast<INT>(m_vertices.size());

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
	bd.ByteWidth = static_cast<UINT>(m_vertices.size() * sizeof(Vertex)); // Size of buffer in bytes
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = m_vertices.data();
	GFX_THROW_INFO(device->CreateBuffer(&bd, &sd, m_vertexBuffer.ReleaseAndGetAddressOf()))

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = static_cast<UINT>(m_indices.size() * sizeof(std::uint16_t));
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

MeshInstance MeshSet::AddBox(float width, float height, float depth, uint32 numSubdivisions)
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");

	MeshData meshData;

	//
	// Create the vertices.
	//
	GeneralVertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = GeneralVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = GeneralVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = GeneralVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = GeneralVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = GeneralVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = GeneralVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = GeneralVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = GeneralVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = GeneralVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = GeneralVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = GeneralVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = GeneralVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = GeneralVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = GeneralVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = GeneralVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = GeneralVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = GeneralVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = GeneralVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = GeneralVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = GeneralVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = GeneralVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = GeneralVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = GeneralVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = GeneralVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	meshData.Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//
	uint32 i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.Indices32.assign(&i[0], &i[36]);

	// Put a cap on the number of subdivisions.
	numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

	for (uint32 i = 0; i < numSubdivisions; ++i)
		Subdivide(meshData);

	std::vector<Vertex> finalVertices = m_VertexConversionFn(meshData.Vertices);
	std::vector<std::uint16_t> finalIndices = meshData.GetIndices16();

	return AddMesh(finalVertices, finalIndices);
}
MeshInstance MeshSet::AddSphere(float radius, uint32 sliceCount, uint32 stackCount)
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");

	MeshInstance mi;
	return mi;
}
MeshInstance MeshSet::AddGeosphere(float radius, uint32 numSubdivisions)
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");

	MeshInstance mi;
	return mi;
}
MeshInstance MeshSet::AddCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount)
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");

	MeshInstance mi;
	return mi;
}
MeshInstance MeshSet::AddGrid(float width, float depth, uint32 m, uint32 n)
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");

	MeshInstance mi;
	return mi;
}
MeshInstance MeshSet::AddQuad(float x, float y, float w, float h, float depth)
{
	EG_ASSERT(!m_finalized, "The MeshSet has already been finalized");

	MeshInstance mi;
	return mi;
}


void MeshSet::Subdivide(MeshData& meshData) const
{
	// Save a copy of the input geometry.
	MeshData inputCopy = meshData;


	meshData.Vertices.resize(0);
	meshData.Indices32.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	uint32 numTris = (uint32)inputCopy.Indices32.size() / 3;
	for (uint32 i = 0; i < numTris; ++i)
	{
		GeneralVertex v0 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 0]];
		GeneralVertex v1 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 1]];
		GeneralVertex v2 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		GeneralVertex m0 = MidPoint(v0, v1);
		GeneralVertex m1 = MidPoint(v1, v2);
		GeneralVertex m2 = MidPoint(v0, v2);

		//
		// Add new geometry.
		//

		meshData.Vertices.push_back(v0); // 0
		meshData.Vertices.push_back(v1); // 1
		meshData.Vertices.push_back(v2); // 2
		meshData.Vertices.push_back(m0); // 3
		meshData.Vertices.push_back(m1); // 4
		meshData.Vertices.push_back(m2); // 5

		meshData.Indices32.push_back(i * 6 + 0);
		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 5);

		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 4);
		meshData.Indices32.push_back(i * 6 + 5);

		meshData.Indices32.push_back(i * 6 + 5);
		meshData.Indices32.push_back(i * 6 + 4);
		meshData.Indices32.push_back(i * 6 + 2);

		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 1);
		meshData.Indices32.push_back(i * 6 + 4);
	}
}

MeshSet::GeneralVertex MeshSet::MidPoint(const GeneralVertex& v0, const GeneralVertex& v1) const
{
	XMVECTOR p0 = XMLoadFloat3(&v0.Position);
	XMVECTOR p1 = XMLoadFloat3(&v1.Position);

	XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
	XMVECTOR n1 = XMLoadFloat3(&v1.Normal);

	XMVECTOR tan0 = XMLoadFloat3(&v0.TangentU);
	XMVECTOR tan1 = XMLoadFloat3(&v1.TangentU);

	XMVECTOR tex0 = XMLoadFloat2(&v0.TexC);
	XMVECTOR tex1 = XMLoadFloat2(&v1.TexC);

	// Compute the midpoints of all the attributes.  Vectors need to be normalized
	// since linear interpolating can make them not unit length.  
	XMVECTOR pos = 0.5f * (p0 + p1);
	XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
	XMVECTOR tangent = XMVector3Normalize(0.5f * (tan0 + tan1));
	XMVECTOR tex = 0.5f * (tex0 + tex1);

	GeneralVertex v;
	XMStoreFloat3(&v.Position, pos);
	XMStoreFloat3(&v.Normal, normal);
	XMStoreFloat3(&v.TangentU, tangent);
	XMStoreFloat2(&v.TexC, tex);

	return v;
}