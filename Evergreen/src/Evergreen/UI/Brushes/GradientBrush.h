#pragma once
#include "pch.h"
#include "ColorBrush.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
enum class GRADIENT_AXIS
{
	HORIZONTAL,
	VERTICAL,
	NESW,
	NWSE
};

// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API GradientBrush : public ColorBrush
{
public:
	// Add parameters after DeviceResources must have a default so that BrushLoader can create the brush
	GradientBrush(std::shared_ptr<DeviceResources> deviceResources, 
		const D2D1_COLOR_F& startColor,
		const D2D1_COLOR_F& endColor,
		GRADIENT_AXIS axis = GRADIENT_AXIS::VERTICAL,
		D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP,
		D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2);
	GradientBrush(std::shared_ptr<DeviceResources> deviceResources,
		const std::vector<D2D1_GRADIENT_STOP>& stops,
		GRADIENT_AXIS axis = GRADIENT_AXIS::VERTICAL,
		D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE::D2D1_EXTEND_MODE_CLAMP,
		D2D1_GAMMA gamma = D2D1_GAMMA::D2D1_GAMMA_2_2);
	GradientBrush(const GradientBrush&);
	GradientBrush& operator=(const GradientBrush&);
	~GradientBrush() noexcept override {}

	void Refresh() override;
	ND std::unique_ptr<ColorBrush> Duplicate() override;

	void Stops(const std::vector<D2D1_GRADIENT_STOP>& stops);
	void Gamma(D2D1_GAMMA gamma);
	void ExtendMode(D2D1_EXTEND_MODE mode);
	void GradientAxis(GRADIENT_AXIS axis);

	ND inline const std::vector<D2D1_GRADIENT_STOP>& Stops() const noexcept { return m_stops; }
	ND inline D2D1_GAMMA Gamma() const noexcept { return m_gamma; }
	ND inline D2D1_EXTEND_MODE ExtendMode() const noexcept { return m_extendMode; }
	ND inline GRADIENT_AXIS GradientAxis() const noexcept { return m_axis; }
	ND inline const D2D1_POINT_2F& GradientAxisStart() const noexcept { return m_gradientAxisStart; }
	ND inline const D2D1_POINT_2F& GradientAxisEnd() const noexcept { return m_gradientAxisEnd; }

protected:
	void OnDrawRegionChanged() override { RefreshGradientAxis(); Refresh(); }

private:
	void RefreshGradientStops();
	void RefreshGradientAxis() noexcept;


	Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> m_pGradientStops;

	std::vector<D2D1_GRADIENT_STOP> m_stops;
	D2D1_GAMMA						m_gamma;
	D2D1_EXTEND_MODE				m_extendMode;

	// Use m_drawRegion (declared in Base class) and Axis to determine start and end for the gradient
	GRADIENT_AXIS					m_axis;

	D2D1_POINT_2F					m_gradientAxisStart;
	D2D1_POINT_2F					m_gradientAxisEnd;
};
#pragma warning( pop )
}