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
		UI* ui,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float radiusX = 5.0f,
		float radiusY = 5.0f,
		float borderWidth = 0.0f,
		const Evergreen::Margin& margin = { 0 });
	RoundedButton(const RoundedButton& text) noexcept = delete;
	void operator=(const RoundedButton&) noexcept = delete;
	virtual ~RoundedButton() noexcept override {}

	// Inherited from Control
	void Render() const override;

	ND inline const D2D1_ROUNDED_RECT& BackgroundRoundedRect() const noexcept { return m_roundedRect; }

	ND bool ContainsPoint(float x, float y) const noexcept override;

	void SetCornerRadius(float xAndY) noexcept { m_radiusX = xAndY; m_radiusY = xAndY; ButtonChanged();  }
	void SetCornerRadius(float x, float y) noexcept { m_radiusX = x; m_radiusY = y; ButtonChanged(); }

protected:
	void ButtonChanged() override;
	virtual void OnAllowedRegionChanged() noexcept override;

	float m_radiusX;
	float m_radiusY;
	D2D1_ROUNDED_RECT m_roundedRect;
	Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> m_roundedRectGeometry;
};
#pragma warning( pop )

}