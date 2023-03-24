#include "ConstantBufferArray.h"

using namespace Evergreen;

ConstantBufferArray::ConstantBufferArray(std::shared_ptr<Evergreen::DeviceResources> deviceResources) noexcept :
	m_deviceResources(deviceResources)
{
}


void ConstantBufferArray::AddBuffer(std::shared_ptr<ConstantBuffer> buffer) noexcept
{
	m_buffers.push_back(buffer);
	m_rawBufferPointers.push_back(m_buffers.back()->GetRawBufferPointer());
}
void ConstantBufferArray::ClearBuffers() noexcept
{
	m_buffers.clear();
	m_rawBufferPointers.clear();
}

void ConstantBufferArray::BindCS() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	GFX_THROW_INFO_ONLY(
		context->CSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindVS() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	GFX_THROW_INFO_ONLY(
		context->VSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindHS() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	GFX_THROW_INFO_ONLY(
		context->HSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindDS() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	GFX_THROW_INFO_ONLY(
		context->DSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindGS() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	GFX_THROW_INFO_ONLY(
		context->GSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindPS() const
{
	auto context = m_deviceResources->D3DDeviceContext();

	GFX_THROW_INFO_ONLY(
		context->PSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}