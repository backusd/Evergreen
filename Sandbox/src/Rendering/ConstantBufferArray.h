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
	ConstantBufferArray(std::shared_ptr<Evergreen::DeviceResources> deviceResources) noexcept;
	ConstantBufferArray(const ConstantBufferArray&) = delete;
	ConstantBufferArray& operator=(const ConstantBufferArray&) = delete;

	void AddBuffer(std::shared_ptr<ConstantBuffer> buffer) noexcept;
	void ClearBuffers() noexcept;

	void BindCS() const;
	void BindVS() const;
	void BindHS() const;
	void BindDS() const;
	void BindGS() const;
	void BindPS() const;

protected:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

	std::vector<ID3D11Buffer*> m_rawBufferPointers;
	std::vector<std::shared_ptr<ConstantBuffer>> m_buffers;
};