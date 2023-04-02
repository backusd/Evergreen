#pragma once
#include "pch.h"
#include <Evergreen.h>

class DepthStencilState
{
public:
	DepthStencilState(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const D3D11_DEPTH_STENCIL_DESC& desc) :
		m_deviceResources(deviceResources)
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");

		auto device = m_deviceResources->D3DDevice();
		GFX_THROW_INFO(device->CreateDepthStencilState(&desc, m_depthStencilState.ReleaseAndGetAddressOf()));
	}
	DepthStencilState(const DepthStencilState&) noexcept = delete;
	void operator=(const DepthStencilState&) noexcept = delete;
	virtual ~DepthStencilState() noexcept {}

	ND inline ID3D11DepthStencilState* Get() { return m_depthStencilState.Get(); }

protected:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
};