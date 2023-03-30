#pragma once
#include "pch.h"
#include <Evergreen.h>

class BlendState
{
public:
	BlendState(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const D3D11_BLEND_DESC& desc) :
		m_deviceResources(deviceResources)
	{
		auto device = m_deviceResources->D3DDevice();
		GFX_THROW_INFO(device->CreateBlendState(&desc, m_blendState.ReleaseAndGetAddressOf()));
	}
	BlendState(const BlendState&) noexcept = delete;
	void operator=(const BlendState&) noexcept = delete;
	virtual ~BlendState() noexcept {}

	ID3D11BlendState* Get() { return m_blendState.Get(); }

protected:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
};