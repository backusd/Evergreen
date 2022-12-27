#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Styles/TextStyle.h"
#include "Evergreen/UI/Brushes/Brushes.h"
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
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float borderWidth = 0.0f,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	//Button(const Button& text) noexcept;
	//void operator=(const Button&) noexcept;
	virtual ~Button() noexcept override {}

	// Inherited from Control
	void Update() const noexcept override {}
	virtual void Render() const noexcept override;
	virtual void OnMouseMove(MouseMoveEvent& e) noexcept override;
	virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	virtual void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;

	ColorBrush* BackgroundBrush() const noexcept { return m_backgroundBrush.get(); }
	ColorBrush* BorderBrush() const noexcept { return m_borderBrush.get(); }
	Layout* GetLayout() const noexcept { return m_layout.get(); }
	float BorderWidth() const noexcept { return m_borderWidth; }
	const D2D1_RECT_F& BackgroundRect() const noexcept { return m_backgroundRect; }

	virtual void BackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_layout->Brush(std::move(brush)); } // Pass the brush to the layout
	void BorderBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_borderBrush = std::move(brush); }
	void BorderWidth(float width) noexcept { m_borderWidth = width; }

	void OnMouseEnter(std::function<void(Button*)> func) noexcept { m_OnMouseEnter = func; }
	void OnMouseLeave(std::function<void(Button*)> func) noexcept { m_OnMouseLeave = func; }
	void OnMouseLButtonDown(std::function<void(Button*)> func) noexcept { m_OnMouseLButtonDown = func; }
	void OnMouseLButtonUp(std::function<void(Button*)> func) noexcept { m_OnMouseLButtonUp = func; }
	void OnClick(std::function<void(Button*)> func) noexcept { m_OnClick = func; }

	bool MouseIsOver() const noexcept { return m_mouseIsOver; }

protected:
	virtual bool ContainsPoint(float x, float y) const noexcept;
	virtual void ButtonChanged();

	virtual void OnMarginChanged() override;
	virtual void OnAllowedRegionChanged() override;

	std::function<void(Button*)> m_OnMouseEnter;
	std::function<void(Button*)> m_OnMouseLeave;
	std::function<void(Button*)> m_OnMouseLButtonDown;
	std::function<void(Button*)> m_OnMouseLButtonUp;
	std::function<void(Button*)> m_OnClick;

	std::unique_ptr<ColorBrush> m_backgroundBrush;
	std::unique_ptr<ColorBrush> m_borderBrush;
	std::unique_ptr<Layout> m_layout;
	float m_borderWidth;
	D2D1_RECT_F m_backgroundRect;

	bool m_mouseIsOver;
	bool m_mouseLButtonIsDown;
};
#pragma warning( pop )



/*
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API RoundedButton : public Button
{
public:
	RoundedButton(std::shared_ptr<DeviceResources> deviceResources) noexcept;
	//RoundedButton(const RoundedButton& text) noexcept;
	//void operator=(const RoundedButton&) noexcept;
	virtual ~RoundedButton() noexcept override {}

	// Inherited from Control
	void Render() const noexcept override;

	const D2D1_ROUNDED_RECT& BackgroundRoundedRect() const noexcept { return m_roundedRect; }


protected:
	float m_radiusX;
	float m_radiusY;
	D2D1_ROUNDED_RECT m_roundedRect;
};
#pragma warning( pop )
*/

}
