#include "RenderObject.h"
#include "Structs.h"

using namespace Evergreen;
using namespace DirectX;

RenderObject::RenderObject(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh) :
	m_deviceResources(deviceResources),
	m_mesh(mesh)
{}


void RenderObject::Render(const Camera& camera)
{
	auto context = m_deviceResources->D3DDeviceContext();
	auto device = m_deviceResources->D3DDevice();

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(ObjectConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	XMMATRIX view = camera.GetViewMatrix();
	XMMATRIX world = XMLoadFloat4x4(&m_world);

	XMMATRIX projection = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, m_aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_proj, projection);
	XMMATRIX worldViewProj = world * view * projection;

	// Update the constant buffer with the latest worldViewProj matrix.
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));

	D3D11_SUBRESOURCE_DATA bufferData;
	bufferData.pSysMem = static_cast<void*>(&objConstants);
	bufferData.SysMemPitch = 0u;
	bufferData.SysMemSlicePitch = 0u;

	GFX_THROW_INFO(device->CreateBuffer(&bufferDesc, &bufferData, buffer.ReleaseAndGetAddressOf()))

		ID3D11Buffer* const buffers[1] = { buffer.Get() };
	GFX_THROW_INFO_ONLY(context->VSSetConstantBuffers(0u, 1u, buffers))

	GFX_THROW_INFO_ONLY(context->DrawIndexed(m_mesh.IndexCount, m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation))
}