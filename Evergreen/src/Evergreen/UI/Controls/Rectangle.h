#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Brushes.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Rectangle : public Control
{
public:
	Rectangle(std::shared_ptr<DeviceResources> deviceResources,
		UI* ui,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		std::unique_ptr<ColorBrush> brush = nullptr,
		float height = 10.0f,
		float width = 10.0f,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	Rectangle(const Rectangle& text) noexcept = delete; // Just delete for now until there is a good use case
	Rectangle& operator=(const Rectangle&) noexcept = delete;
	virtual ~Rectangle() noexcept override {}

	// Inherited from Control
	void Render() const override;

	ND inline Control* GetControlByName(const std::string& name) noexcept override;
	ND inline Control* GetControlByID(unsigned int id) noexcept override;

	// Button specific
	ND inline ColorBrush* GetBrush() const noexcept { return m_brush.get(); }
	ND inline const D2D1_RECT_F& GetPosition() const noexcept { return m_backgroundRect; }

	inline void SetBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_brush = std::move(brush); }

	inline ControlType GetControlType() const noexcept override { return ControlType::Rectangle; }

protected:

	void RectangleChanged();
	inline void OnMarginChanged() override { RectangleChanged(); }
	inline void OnAllowedRegionChanged() override { RectangleChanged(); }

	std::unique_ptr<ColorBrush> m_brush;
	D2D1_RECT_F					m_backgroundRect; 
	float						m_height;
	float						m_width; 
};
#pragma warning( pop )
}
