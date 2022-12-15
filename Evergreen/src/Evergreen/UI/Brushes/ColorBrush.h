#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
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
	ColorBrush(std::shared_ptr<DeviceResources> deviceResources, 
		const D2D1_RECT_F& drawRegion = D2D1::RectF(0.0f, 0.0f, 1000.0f, 1000.0f)) noexcept;
	ColorBrush(const ColorBrush&) noexcept;
	void operator=(const ColorBrush&) noexcept;

	std::shared_ptr<DeviceResources> GetDeviceResources() const noexcept { return m_deviceResources; }
	ID2D1Brush* Get() const noexcept;

	virtual void Refresh() noexcept = 0;

	void SetOpacity(float opacity) noexcept { m_brushProperties.opacity = opacity; Refresh(); }
	void SetTransform(const D2D1_MATRIX_3X2_F& transform) noexcept { m_brushProperties.transform = transform; Refresh(); }
	void SetDrawRegion(const D2D1_RECT_F& rect) noexcept { m_drawRegion = rect; OnDrawRegionChanged(); }

	const D2D1_RECT_F& GetDrawingRect() const noexcept { return m_drawRegion; }

protected:
	virtual void OnDrawRegionChanged() noexcept {}

	std::shared_ptr<DeviceResources>	m_deviceResources;
	Microsoft::WRL::ComPtr<ID2D1Brush>	m_brush;
	D2D1_BRUSH_PROPERTIES				m_brushProperties;

	// The draw region will have different interpretations depending on the derived type
	// SolidColorBrush		- Does not use the draw region
	// GradientColorBrush	- Draw region represents the area the gradient brush will be drawn to
	// RadialBrush			- Draw region width and height represent the x,y radius respectively
	// BitmapBrush			- Draw region represents the area the bitmap will be drawn to
	D2D1_RECT_F	m_drawRegion;
};
#pragma warning( pop )
}