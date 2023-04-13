#pragma once
#include "pch.h"
#include <Evergreen.h>




class ConstantBuffer
{
public:
	ConstantBuffer(std::shared_ptr<Evergreen::DeviceResources> deviceResources) noexcept :
		m_deviceResources(deviceResources)
	{}
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;

	template <typename T>
	void CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride);
	template <typename T>
	void CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride, void* initialData);

	ID3D11Buffer* GetRawBufferPointer() const { return m_buffer.Get(); }

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};

template <typename T>
void ConstantBuffer::CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride)
{
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