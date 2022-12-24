#pragma once
#include "pch.h"
#include "Evergreen/UI/Controls/Button.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API RoundedButton : public Button
{
public:
	RoundedButton(std::shared_ptr<DeviceResources> deviceResources,
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float radiusX = 5.0f,
		float radiusY = 5.0f,
		float borderWidth = 0.0f,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	//RoundedButton(const RoundedButton& text) noexcept;
	//void operator=(const RoundedButton&) noexcept;
	virtual ~RoundedButton() noexcept override {}

	// Inherited from Control
	void Render() const noexcept override;

	const D2D1_ROUNDED_RECT& BackgroundRoundedRect() const noexcept { return m_roundedRect; }


protected:
	void ButtonChanged() noexcept override;


	float m_radiusX;
	float m_radiusY;
	D2D1_ROUNDED_RECT m_roundedRect;
};
#pragma warning( pop )

}