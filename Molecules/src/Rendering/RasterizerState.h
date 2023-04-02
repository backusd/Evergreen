#pragma once
#include "pch.h"
#include <Evergreen.h>

class RasterizerState
{
public:
	RasterizerState(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const D3D11_RASTERIZER_DESC& desc) :
		m_deviceResources(deviceResources)
	{
		EG_ASSERT(m_deviceResources != nullptr, "No device resources");

		auto device = m_deviceResources->D3DDevice();
		GFX_THROW_INFO(device->CreateRasterizerState(&desc, m_rasterizerState.ReleaseAndGetAddressOf()));
	}
	RasterizerState(const RasterizerState&) noexcept = delete;
	void operator=(const RasterizerState&) noexcept = delete;
	virtual ~RasterizerState() noexcept {}

	ND inline ID3D11RasterizerState* Get() { return m_rasterizerState.Get(); }

protected:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
};