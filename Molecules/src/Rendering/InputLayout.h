#pragma once
#include "pch.h"
#include <Evergreen.h>
#include "Shaders.h"

class InputLayout
{
public:
	InputLayout(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements, VertexShader* vs) :
		m_deviceResources(deviceResources)
	{
		auto device = m_deviceResources->D3DDevice();
		GFX_THROW_INFO(
			device->CreateInputLayout(
				inputElements.data(),
				static_cast<UINT>(inputElements.size()),
				vs->GetBufferPointer(),
				vs->GetBufferSize(),
				&m_inputLayout
			)
		);
	}
	InputLayout(const InputLayout&) noexcept = delete;
	void operator=(const InputLayout&) noexcept = delete;
	virtual ~InputLayout() noexcept {}

	ID3D11InputLayout* Get() { return m_inputLayout.Get(); }

protected:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};