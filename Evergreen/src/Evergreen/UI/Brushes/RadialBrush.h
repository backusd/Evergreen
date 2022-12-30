#pragma once
#include "pch.h"
#include "ColorBrush.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API RadialBrush : public ColorBrush
{
public:
	// Add parameters after DeviceResources must have a default so that BrushLoader can create the brush
	RadialBrush(std::shared_ptr<DeviceResources> deviceResources,
		const D2D1_COLOR_F& startColor,
		const D2D1_COLOR_F& endColor,
		const D2D1_POINT_2F& originOffset = D2D1::Point2F(),
		D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP,
		D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2) noexcept;
	RadialBrush(std::shared_ptr<DeviceResources> deviceResources,
		const std::vector<D2D1_GRADIENT_STOP>& stops,
		const D2D1_POINT_2F& originOffset = D2D1::Point2F(),
		D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP,
		D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2) noexcept;
	RadialBrush(const RadialBrush&) noexcept;
	void operator=(const RadialBrush&) noexcept;
	~RadialBrush() noexcept override {}

	void Refresh() noexcept override;
	ND std::unique_ptr<ColorBrush> Duplicate() noexcept override;

	void Stops(const std::vector<D2D1_GRADIENT_STOP>& stops) noexcept;
	void Gamma(D2D1_GAMMA gamma) noexcept;
	void ExtendMode(D2D1_EXTEND_MODE mode) noexcept;
	void GradientOriginOffset(const D2D1_POINT_2F& offset) noexcept;

	ND inline float RadiusX() const noexcept { return (m_drawRegion.right - m_drawRegion.left) / 2.0f; }
	ND inline float RadiusY() const noexcept { return (m_drawRegion.bottom - m_drawRegion.top) / 2.0f; }
	ND inline D2D1_POINT_2F Center() const noexcept { return D2D1::Point2F(m_drawRegion.left + RadiusX(), m_drawRegion.top + RadiusY()); }
	ND inline const std::vector<D2D1_GRADIENT_STOP>& Stops() const noexcept { return m_stops; }
	ND inline D2D1_GAMMA Gamma() const noexcept { return m_gamma; }
	ND inline D2D1_EXTEND_MODE ExtendMode() const noexcept { return m_extendMode; }
	ND inline const D2D1_POINT_2F& GradientOriginOffset() const noexcept { return m_gradientOriginOffset; }

protected:
	void OnDrawRegionChanged() noexcept override { Refresh(); }

private:
	void RefreshGradientStops() noexcept;

	Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> m_pGradientStops;

	std::vector<D2D1_GRADIENT_STOP> m_stops;
	D2D1_GAMMA						m_gamma;
	D2D1_EXTEND_MODE				m_extendMode;
	D2D1_POINT_2F					m_gradientOriginOffset;

	// No need for center, radiusX, or radiusY. We will force use of m_drawRegion (see base class) and
	// use the center of that region, width, and height for these values, respectively

};
#pragma warning( pop )
}