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
class EVERGREEN_API GradientBrush : public ColorBrush
{
public:
	GradientBrush(std::shared_ptr<DeviceResources> deviceResources, 
		const D2D1_COLOR_F& startColor, const D2D1_COLOR_F& endColor, 
		const D2D1_POINT_2F& gradientAxisStart, const D2D1_POINT_2F& gradientAxisEnd,
		D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP,
		D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2) noexcept;
	GradientBrush(std::shared_ptr<DeviceResources> deviceResources,
		const std::vector<D2D1_GRADIENT_STOP>& stops,
		const D2D1_POINT_2F& gradientAxisStart, const D2D1_POINT_2F& gradientAxisEnd,
		D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP,
		D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2) noexcept;
	GradientBrush(const GradientBrush&) noexcept;
	void operator=(const GradientBrush&) noexcept;

	void Refresh() noexcept override;

	void ClearStops() noexcept { m_stops.clear(); }
	void AddStop(const D2D1_GRADIENT_STOP& stop) noexcept { m_stops.push_back(stop); RefreshGradientStops(); }
	void Gamma(D2D1_GAMMA gamma) noexcept { m_gamma = gamma; RefreshGradientStops(); }
	void ExtendMode(D2D1_EXTEND_MODE mode) noexcept { m_extendMode = mode; RefreshGradientStops(); }
	void GradientAxisStart(const D2D1_POINT_2F& start) noexcept { m_gradientAxisStart = start; Refresh(); }
	void GradientAxisEnd(const D2D1_POINT_2F& end) noexcept { m_gradientAxisEnd = end; Refresh(); }
	void GradientAxis(const D2D1_POINT_2F& start, const D2D1_POINT_2F& end) noexcept { m_gradientAxisStart = start; m_gradientAxisEnd = end; Refresh(); }


	const std::vector<D2D1_GRADIENT_STOP>& Stops() const noexcept { return m_stops; }
	D2D1_GAMMA Gamma() const noexcept { return m_gamma; }
	D2D1_EXTEND_MODE ExtendMode() const noexcept { return m_extendMode; }
	const D2D1_POINT_2F& GradientAxisStart() const noexcept { return m_gradientAxisStart; }
	const D2D1_POINT_2F& GradientAxisEnd() const noexcept { return m_gradientAxisEnd; }

private:
	void RefreshGradientStops() noexcept;

	Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> m_pGradientStops;

	std::vector<D2D1_GRADIENT_STOP> m_stops;
	D2D1_GAMMA						m_gamma;
	D2D1_EXTEND_MODE				m_extendMode;
	D2D1_POINT_2F					m_gradientAxisStart;
	D2D1_POINT_2F					m_gradientAxisEnd;


};
#pragma warning( pop )
}