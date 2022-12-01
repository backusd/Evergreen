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
	RadialBrush(std::shared_ptr<DeviceResources> deviceResources,
		const D2D1_COLOR_F& startColor, const D2D1_COLOR_F& endColor,
		const D2D1_POINT_2F& center, float radiusX, float radiusY,
		D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP,
		D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2) noexcept;
	RadialBrush(std::shared_ptr<DeviceResources> deviceResources,
		const std::vector<D2D1_GRADIENT_STOP>& stops,
		const D2D1_POINT_2F& center,
		float radiusX, float radiusY,
		const D2D1_POINT_2F& gradientOriginOffset = D2D1::Point2F(),
		D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP,
		D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2) noexcept;
	RadialBrush(const RadialBrush&) noexcept;
	void operator=(const RadialBrush&) noexcept;

	void Refresh() noexcept override;

	void ClearStops() noexcept { m_stops.clear(); }
	void AddStop(const D2D1_GRADIENT_STOP& stop) noexcept { m_stops.push_back(stop); RefreshGradientStops(); }
	void Gamma(D2D1_GAMMA gamma) noexcept { m_gamma = gamma; RefreshGradientStops(); }
	void ExtendMode(D2D1_EXTEND_MODE mode) noexcept { m_extendMode = mode; RefreshGradientStops(); }
	void Center(const D2D1_POINT_2F& center) noexcept { m_center = center; Refresh(); }
	void GradientOriginOffset(const D2D1_POINT_2F& offset) noexcept { m_gradientOriginOffset = offset; Refresh(); }
	void Radius(float radiusXAndY) noexcept { m_radiusX = radiusXAndY; m_radiusY = radiusXAndY; Refresh(); }
	void Radius(float radiusX, float radiusY) noexcept { m_radiusX = radiusX; m_radiusY = radiusY; Refresh(); }
	void RadiusX(float radiusX) noexcept { m_radiusX = radiusX; Refresh(); }
	void RadiusY(float radiusY) noexcept { m_radiusY = radiusY; Refresh(); }

	const std::vector<D2D1_GRADIENT_STOP>& Stops() const noexcept { return m_stops; }
	D2D1_GAMMA Gamma() const noexcept { return m_gamma; }
	D2D1_EXTEND_MODE ExtendMode() const noexcept { return m_extendMode; }
	const D2D1_POINT_2F& Center() const noexcept { return m_center; }
	const D2D1_POINT_2F& GradientOriginOffset() const noexcept { return m_gradientOriginOffset; }
	float RadiusX() const noexcept { return m_radiusX; }
	float RadiusY() const noexcept { return m_radiusY; }

private:
	void RefreshGradientStops() noexcept;

	Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> m_pGradientStops;

	std::vector<D2D1_GRADIENT_STOP> m_stops;
	D2D1_GAMMA						m_gamma;
	D2D1_EXTEND_MODE				m_extendMode;
	D2D1_POINT_2F					m_center;
	D2D1_POINT_2F					m_gradientOriginOffset;
	float							m_radiusX;
	float							m_radiusY;


};
#pragma warning( pop )
}