#include "Shaders.h"

using namespace Evergreen;

Shader::Shader(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename) :
	m_deviceResources(deviceResources),
	m_filename(filename)
{}

// --------------------------------------------------------------------------------------------------

VertexShader::VertexShader(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename) :
	Shader(deviceResources, filename)
{
	auto device = m_deviceResources->D3DDevice();

	// Create Pixel Shader
	D3DReadFileToBlob(filename.c_str(), &m_blob);
	GFX_THROW_INFO(device->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_vertexShader));
}

// --------------------------------------------------------------------------------------------------

PixelShader::PixelShader(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename) :
	Shader(deviceResources, filename)
{
	auto device = m_deviceResources->D3DDevice();

	// Create Pixel Shader
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(filename.c_str(), &pBlob);
	GFX_THROW_INFO(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pixelShader));
}