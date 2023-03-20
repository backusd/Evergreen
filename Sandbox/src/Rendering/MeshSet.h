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
	MeshSet(std::shared_ptr<Evergreen::DeviceResources> deviceResources);

	MeshInstance AddMesh(const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices);
	void Finalize();

	void BindToIA() const;

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	std::vector<Vertex>			m_vertices;
	std::vector<std::uint16_t>	m_indices;

	bool m_finalized;
};