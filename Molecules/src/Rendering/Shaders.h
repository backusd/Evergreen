#pragma once
#include "pch.h"
#include <Evergreen.h>


class Shader
{
public:
	Shader(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const std::wstring& filename) noexcept;
	Shader(const Shader&) noexcept = delete;
	void operator=(const Shader&) noexcept = delete;
	virtual ~Shader() noexcept {}

protected:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;
	std::wstring m_filename;
};

// -------------------------------------------------------------------------

class VertexShader : public Shader
{
public:
	VertexShader(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const std::wstring& filename);
	VertexShader(const VertexShader&) noexcept = delete;
	void operator=(const VertexShader&) noexcept = delete;
	virtual ~VertexShader() noexcept override {}

	ND inline ID3D11VertexShader* Get() noexcept { return m_vertexShader.Get(); }
	ND inline void* GetBufferPointer() { return m_blob->GetBufferPointer(); }
	ND inline SIZE_T GetBufferSize() { return m_blob->GetBufferSize(); }

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob>			m_blob;
};

// -------------------------------------------------------------------------

class PixelShader : public Shader
{
public:
	PixelShader(std::shared_ptr<Evergreen::DeviceResources> deviceResources, const std::wstring& filename);
	PixelShader(const PixelShader&) noexcept = delete;
	void operator=(const PixelShader&) noexcept = delete;
	virtual ~PixelShader() noexcept override {}

	ND inline ID3D11PixelShader* Get() noexcept { return m_pixelShader.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
}; 