#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Styles/TextStyle.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/UI/Layout.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Button : public Control
{
public:
	Button(std::shared_ptr<DeviceResources> deviceResources,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float borderWidth = 0.0f,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	Button(const Button& text) noexcept = delete; // Just delete for now until there is a good use case
	void operator=(const Button&) noexcept = delete;
	virtual ~Button() noexcept override {}

	// Inherited from Control
	void Update() const noexcept override {}
	virtual void Render() const noexcept override;
	virtual void OnMouseMove(MouseMoveEvent& e) noexcept override;
	virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	virtual void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;

	// Button specific
	ND inline virtual ColorBrush* BackgroundBrush() const noexcept { return m_layout->Brush(); }
	ND inline ColorBrush* BorderBrush() const noexcept { return m_borderBrush.get(); }
	ND inline Layout* GetLayout() const noexcept { return m_layout.get(); }
	ND inline float BorderWidth() const noexcept { return m_borderWidth; }
	ND inline const D2D1_RECT_F& BackgroundRect() const noexcept { return m_backgroundRect; }

	virtual void BackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_layout->Brush(std::move(brush)); } // Pass the brush to the layout
	void BorderBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_borderBrush = std::move(brush); }
	void BorderWidth(float width) noexcept { m_borderWidth = width; }

	void OnMouseEnter(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseEnter = func; }
	void OnMouseLeave(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseLeave = func; }
	void OnMouseMoved(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseMoved = func; }
	void OnMouseLButtonDown(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseLButtonDown = func; }
	void OnMouseLButtonUp(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseLButtonUp = func; }
	void OnClick(std::function<void(Control*, Event& e)> func) noexcept { m_OnClick = func; }

	ND inline bool MouseIsOver() const noexcept { return m_mouseIsOver; }

protected:
	ND virtual bool ContainsPoint(float x, float y) const noexcept;
	virtual void ButtonChanged();

	virtual void OnMarginChanged() override;
	virtual void OnAllowedRegionChanged() override;

	std::function<void(Control*, Event&)> m_OnMouseEnter = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnMouseLeave = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnMouseMoved = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnMouseLButtonDown = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnMouseLButtonUp = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnClick = [](Control*, Event&) {};

	std::unique_ptr<ColorBrush> m_backgroundBrush;
	std::unique_ptr<ColorBrush> m_borderBrush;
	std::unique_ptr<Layout>		m_layout;
	float						m_borderWidth;
	D2D1_RECT_F					m_backgroundRect;

	bool m_mouseIsOver;
	bool m_mouseLButtonIsDown;
};
#pragma warning( pop )

}
