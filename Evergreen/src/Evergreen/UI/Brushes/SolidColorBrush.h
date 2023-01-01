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
class EVERGREEN_API SolidColorBrush : public ColorBrush
{
public:
	// Add parameters after DeviceResources must have a default so that BrushLoader can create the brush
	SolidColorBrush(std::shared_ptr<DeviceResources> deviceResources, const D2D1_COLOR_F& solidColor);
	SolidColorBrush(const SolidColorBrush&);
	void operator=(const SolidColorBrush&);
	~SolidColorBrush() noexcept override {}

	void Refresh() override;
	ND std::unique_ptr<ColorBrush> Duplicate() override;

	void Color(float r, float g, float b, float a = 1.0f);
	void Color(D2D1::ColorF::Enum colorEnum);
	void Color(const D2D1_COLOR_F& color);

	ND inline const D2D1_COLOR_F& Color() const noexcept { return m_color; }

private:
	D2D1_COLOR_F m_color;
};
#pragma warning( pop )
}