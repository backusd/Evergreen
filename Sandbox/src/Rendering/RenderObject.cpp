#include "RenderObject.h"
#include "Structs.h"

using namespace Evergreen;
using namespace DirectX;

using Microsoft::WRL::ComPtr;

RenderObject::RenderObject(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const MeshInstance& mesh) :
	m_deviceResources(deviceResources),
	m_mesh(mesh)
{
	auto device = m_deviceResources->D3DDevice();

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(ObjectConstants);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	GFX_THROW_INFO(device->CreateBuffer(&bufferDesc, nullptr, m_modelViewProjectionBuffer.ReleaseAndGetAddressOf()))
}

void RenderObject::Update(const Timer& timer, const Camera& camera)
{
	auto context = m_deviceResources->D3DDeviceContext();
	
	// Compute the new World-View-Projection matrix
	XMMATRIX view = camera.GetViewMatrix();
	XMMATRIX world = XMLoadFloat4x4(&m_world);
	XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, m_aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_proj, projection);
	XMMATRIX worldViewProj = world * view * projection;

	// Update the constant buffer with the new matrix values
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	GFX_THROW_INFO(context->Map(m_modelViewProjectionBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));

	ObjectConstants* objConstants = (ObjectConstants*)ms.pData;
	XMStoreFloat4x4(&(objConstants->WorldViewProj), DirectX::XMMatrixTranspose(worldViewProj));

	GFX_THROW_INFO_ONLY(context->Unmap(m_modelViewProjectionBuffer.Get(), 0));
}

void RenderObject::Render(const Camera& camera)
{
	auto context = m_deviceResources->D3DDeviceContext();

	ID3D11Buffer* const buffers[1] = { m_modelViewProjectionBuffer.Get() };
	GFX_THROW_INFO_ONLY(context->VSSetConstantBuffers(0u, 1u, buffers))

	GFX_THROW_INFO_ONLY(context->DrawIndexed(m_mesh.IndexCount, m_mesh.StartIndexLocation, m_mesh.BaseVertexLocation))
}