#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "ConstantBuffer.h"


enum class ShaderStage
{
	COMPUTE_SHADER = 0,
	VERTEX_SHADER = 1,
	HULL_SHADER = 2,
	DOMAIN_SHADER = 3,
	GEOMETRY_SHADER = 4,
	PIXEL_SHADER = 5
};

class ConstantBufferArray
{
public:
	ConstantBufferArray(std::shared_ptr<Evergreen::DeviceResources> deviceResources) noexcept :
		m_deviceResources(deviceResources)
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
	}
	ConstantBufferArray(const ConstantBufferArray&) = delete;
	ConstantBufferArray& operator=(const ConstantBufferArray&) = delete;
	~ConstantBufferArray() noexcept {}

	inline void AddBuffer(std::shared_ptr<ConstantBuffer> buffer) noexcept
	{
		EG_ASSERT(buffer != nullptr, "Buffer cannot be nullptr");
		m_buffers.push_back(buffer);
		m_rawBufferPointers.push_back(m_buffers.back()->GetRawBufferPointer());
	}
	inline void ClearBuffers() noexcept
	{
		m_buffers.clear();
		m_rawBufferPointers.clear();
	}

	inline void BindCS() const
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
		auto context = m_deviceResources->D3DDeviceContext();
		GFX_THROW_INFO_ONLY(context->CSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data()));
	}
	inline void BindVS() const
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
		auto context = m_deviceResources->D3DDeviceContext();
		GFX_THROW_INFO_ONLY(context->VSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data()));
	}
	inline void BindHS() const
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
		auto context = m_deviceResources->D3DDeviceContext();
		GFX_THROW_INFO_ONLY(context->HSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data()));
	}
	inline void BindDS() const
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
		auto context = m_deviceResources->D3DDeviceContext();
		GFX_THROW_INFO_ONLY(context->DSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data()));
	}
	inline void BindGS() const
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
		auto context = m_deviceResources->D3DDeviceContext();
		GFX_THROW_INFO_ONLY(context->GSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data()));
	}
	inline void BindPS() const
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");
		auto context = m_deviceResources->D3DDeviceContext();
		GFX_THROW_INFO_ONLY(context->PSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data()));
	}

protected:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	std::vector<ID3D11Buffer*> m_rawBufferPointers;
	std::vector<std::shared_ptr<ConstantBuffer>> m_buffers;
};