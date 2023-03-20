#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "Structs.h"

struct MeshInstance
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// Bounding box of the geometry defined by this submesh. 
	// This is used in later chapters of the book.
	//DirectX::BoundingBox Bounds;
};


class MeshSet
{
public:
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

	struct GeneralVertex
	{
		GeneralVertex() {}
		GeneralVertex(
			const DirectX::XMFLOAT3& p,
			const DirectX::XMFLOAT3& n,
			const DirectX::XMFLOAT3& t,
			const DirectX::XMFLOAT2& uv) :
			Position(p),
			Normal(n),
			TangentU(t),
			TexC(uv) {}
		GeneralVertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			Position(px, py, pz),
			Normal(nx, ny, nz),
			TangentU(tx, ty, tz),
			TexC(u, v) {}

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 TangentU;
		DirectX::XMFLOAT2 TexC;
	};

	struct MeshData
	{
		std::vector<GeneralVertex> Vertices;
		std::vector<uint32> Indices32;

		std::vector<uint16>& GetIndices16()
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
					mIndices16[i] = static_cast<uint16>(Indices32[i]);
			}

			return mIndices16;
		}

	private:
		std::vector<uint16> mIndices16;
	};

public:
	MeshSet(std::shared_ptr<Evergreen::DeviceResources> deviceResources);

	MeshInstance AddMesh(const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices);
	
	MeshInstance AddBox(float width, float height, float depth, uint32 numSubdivisions);
	MeshInstance AddSphere(float radius, uint32 sliceCount, uint32 stackCount);
	MeshInstance AddGeosphere(float radius, uint32 numSubdivisions);
	MeshInstance AddCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);
	MeshInstance AddGrid(float width, float depth, uint32 m, uint32 n);
	MeshInstance AddQuad(float x, float y, float w, float h, float depth);

	void Finalize();

	void SetVertexConversionFunction(std::function<std::vector<Vertex>(std::vector<GeneralVertex>)> fn) noexcept { m_VertexConversionFn = fn; }

	void BindToIA() const;

private:
	void Subdivide(MeshData& meshData) const;
	GeneralVertex MidPoint(const GeneralVertex& v0, const GeneralVertex& v1) const;
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);

	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	std::function<std::vector<Vertex>(std::vector<GeneralVertex>)> m_VertexConversionFn;
	bool m_conversionFunctionIsSet;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	std::vector<Vertex>			m_vertices;
	std::vector<std::uint16_t>	m_indices;

	bool m_finalized;
};