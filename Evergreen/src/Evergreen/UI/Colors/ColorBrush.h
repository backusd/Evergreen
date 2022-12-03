#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Rendering/DeviceResources.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ColorBrush
{
public:
	ColorBrush(std::shared_ptr<DeviceResources> deviceResources) noexcept;
	ColorBrush(const ColorBrush&) noexcept;
	void operator=(const ColorBrush&) noexcept;

	std::shared_ptr<DeviceResources> GetDeviceResources() const noexcept { return m_deviceResources; }
	ID2D1Brush* Get() const noexcept { return m_brush.Get(); }

	virtual void Refresh() noexcept = 0;

	void SetOpacity(float opacity) noexcept { m_brushProperties.opacity = opacity; Refresh(); }
	void SetTransform(D2D1_MATRIX_3X2_F transform) noexcept { m_brushProperties.transform = transform; Refresh(); }

protected:
	std::shared_ptr<DeviceResources>	m_deviceResources;
	Microsoft::WRL::ComPtr<ID2D1Brush>	m_brush;
	D2D1_BRUSH_PROPERTIES				m_brushProperties;
};
#pragma warning( pop )
}