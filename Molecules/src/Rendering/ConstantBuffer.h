#pragma once
#include "pch.h"
#include <Evergreen.h>

class ConstantBuffer
{
public:
	ConstantBuffer(std::shared_ptr<Evergreen::DeviceResources> deviceResources) noexcept :
		m_deviceResources(deviceResources)
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
	}
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;
	~ConstantBuffer() noexcept {}

	template <typename T>
	void CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride);
	template <typename T>
	void CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride, void* initialData);

	ND inline ID3D11Buffer* GetRawBufferPointer() const { return m_buffer.Get(); }

	inline void UpdateData(void* data)
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
		auto context = m_deviceResources->D3DDeviceContext();

		GFX_THROW_INFO_ONLY(
			context->UpdateSubresource(
				m_buffer.Get(), // Resource to be updated
				0u,				// 0-based index that identified the destination subresource. Will always be 0 when updating a single constant buffer
				nullptr,		// D3D11_BOX that defines the portion of the subresource to copy. "For a shader-constant buffer, set pDstBox to NULL"
				data,			// Pointer to data to copy to the subresource
				0u,				// "SourceRowPitch" - Not really sure what this is for, but it can be 0 for constant buffers
				0u				// "SourceDepthPitch" - Not really sure what this is for, but it can be 0 for constant buffers
			)
		);
	}

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};

template <typename T>
void ConstantBuffer::CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride)
{
	EG_ASSERT(m_deviceResources != nullptr, "No device resources");

	auto device = m_deviceResources->D3DDevice();

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(T);
	desc.Usage = usage;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = cpuAccessFlags;
	desc.MiscFlags = miscFlags;
	desc.StructureByteStride = structuredByteStride;

	GFX_THROW_INFO(
		device->CreateBuffer(
			&desc,								// Use the description we just created
			nullptr,							// Don't fill it with any data
			m_buffer.ReleaseAndGetAddressOf()	// Assign result to buffer
		)
	);
}

template <typename T>
void ConstantBuffer::CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride, void* initialData)
{
	EG_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_ASSERT(initialData != nullptr, "Do not use this version of CreateBuffer if initial data is nullptr");

	auto device = m_deviceResources->D3DDevice();

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(T);
	desc.Usage = usage;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = cpuAccessFlags;
	desc.MiscFlags = miscFlags;
	desc.StructureByteStride = structuredByteStride;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initialData;
	data.SysMemPitch = 0;		// Only relevant for 2D/3D textures
	data.SysMemSlicePitch = 0;	// Only relevant for 2D/3D textures

	GFX_THROW_INFO(
		device->CreateBuffer(
			&desc,								// Use the description we just created
			&data,								// Fill the buffer with the passed in data
			m_buffer.ReleaseAndGetAddressOf()	// Assign result to buffer
		)
	);
}