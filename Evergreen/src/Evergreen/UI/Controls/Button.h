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
		UI* ui,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		const std::array<float, 4>& borderWidths = {},
		const Evergreen::Margin& margin = { 0 }) noexcept;
	Button(std::shared_ptr<DeviceResources> deviceResources,
		UI* ui,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float borderWidth = 0.0f,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	Button(const Button& text) noexcept = delete; // Just delete for now until there is a good use case
	Button& operator=(const Button&) noexcept = delete;
	virtual ~Button() noexcept override {}

	virtual void OnMouseEntered(MouseMoveEvent&) = 0;
	virtual void OnMouseExited(MouseMoveEvent&) = 0;
	virtual void OnMouseMoved(MouseMoveEvent&) = 0;
	virtual void OnMouseLButtonDown(MouseButtonPressedEvent&) = 0;
	virtual void OnMouseLButtonUp(MouseButtonReleasedEvent&) = 0;	
	virtual void OnClick(MouseButtonReleasedEvent&) = 0;

	// Inherited from Control
	virtual void OnUpdate(const Timer& timer) override;
	virtual void Render() const override;

	// There is a special case where a Pane has handled a move event, but this Control was previously handling
	// the move events. Therefore, we need to inform this Control that the mouse is no longer over the Control
	void MouseMoveHandledByPane(MouseMoveEvent& e) override;

	ND inline virtual Layout* GetLayoutByName(const std::string& name) noexcept override { return m_layout->GetLayoutByName(name); }
	ND inline virtual Layout* GetLayoutByID(unsigned int id) noexcept override { return m_layout->GetLayoutByID(id); }
	ND virtual Control* GetControlByName(const std::string& name) noexcept override;
	ND virtual Control* GetControlByID(unsigned int id) noexcept override;


	// Event handling
	virtual void OnChar(CharEvent& e) override;
	virtual void OnKeyPressed(KeyPressedEvent& e) override;
	virtual void OnKeyReleased(KeyReleasedEvent& e) override;
	virtual void OnMouseMove(MouseMoveEvent& e) override;
	virtual void OnMouseScrolledVertical(MouseScrolledEvent& e) override;
	virtual void OnMouseScrolledHorizontal(MouseScrolledEvent& e) override;
	virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) override;
	virtual void OnMouseButtonReleased(MouseButtonReleasedEvent& e) override;
	virtual void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) override;

	// Button specific
	ND inline ColorBrush* BackgroundBrush() const noexcept { return m_backgroundBrush.get(); }
	ND inline ColorBrush* BorderBrush() const noexcept { return m_borderBrush.get(); }
	ND inline Layout* GetLayout() const noexcept { return m_layout.get(); }
	ND inline std::array<float, 4> BorderWidth() const noexcept { return m_borderWidths; }
	ND inline const D2D1_RECT_F& BackgroundRect() const noexcept { return m_backgroundRect; }

	void BackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_backgroundBrush = std::move(brush); }
	void BorderBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_borderBrush = std::move(brush); }
	void BorderWidth(float widthAll) noexcept { m_borderWidths.fill(widthAll); }
	void BorderWidth(const std::array<float, 4>& width) noexcept { m_borderWidths = width; }
	inline void BorderTopLeftOffsetX(float offset) noexcept { m_borderTopLeftOffsetX = offset; }
	inline void BorderTopLeftOffsetY(float offset) noexcept { m_borderTopLeftOffsetY = offset; }
	inline void BorderTopRightOffsetX(float offset) noexcept { m_borderTopRightOffsetX = offset; }
	inline void BorderTopRightOffsetY(float offset) noexcept { m_borderTopRightOffsetY = offset; }
	inline void BorderBottomLeftOffsetX(float offset) noexcept { m_borderBottomLeftOffsetX = offset; }
	inline void BorderBottomLeftOffsetY(float offset) noexcept { m_borderBottomLeftOffsetY = offset; }
	inline void BorderBottomRightOffsetX(float offset) noexcept { m_borderBottomRightOffsetX = offset; }
	inline void BorderBottomRightOffsetY(float offset) noexcept { m_borderBottomRightOffsetY = offset; }

//	inline void SetOnMouseEnteredCallback(std::function<void(Button*, MouseMoveEvent& e)> func) noexcept { m_OnMouseEntered = func; }
//	inline void SetOnMouseExitedCallback(std::function<void(Button*, MouseMoveEvent& e)> func) noexcept { m_OnMouseExited = func; }
//	inline void SetOnMouseMovedCallback(std::function<void(Button*, MouseMoveEvent& e)> func) noexcept { m_OnMouseMoved = func; }
//	inline void SetOnMouseLButtonDownCallback(std::function<void(Button*, MouseButtonPressedEvent& e)> func) noexcept { m_OnMouseLButtonDown = func; }
//	inline void SetOnMouseLButtonUpCallback(std::function<void(Button*, MouseButtonReleasedEvent& e)> func) noexcept { m_OnMouseLButtonUp = func; }
//	inline void SetOnClickCallback(std::function<void(Button*, MouseButtonReleasedEvent& e)> func) noexcept { m_OnClick = func; }

	ND inline bool MouseIsOver() const noexcept { return m_mouseIsOver; }

	ND virtual bool ContainsPoint(float x, float y) const noexcept;

	virtual ControlType GetControlType() const noexcept override { return ControlType::Button; }

protected:
	
	virtual void ButtonChanged();

	virtual void OnMarginChanged() override;
	virtual void OnAllowedRegionChanged() override;

//	std::function<void(Button*, MouseMoveEvent&)> m_OnMouseEntered = [](Button*, MouseMoveEvent&) {};
//	std::function<void(Button*, MouseMoveEvent&)> m_OnMouseExited = [](Button*, MouseMoveEvent&) {};
//	std::function<void(Button*, MouseMoveEvent&)> m_OnMouseMoved = [](Button*, MouseMoveEvent&) {};
//	std::function<void(Button*, MouseButtonPressedEvent&)> m_OnMouseLButtonDown = [](Button*, MouseButtonPressedEvent&) {};
//	std::function<void(Button*, MouseButtonReleasedEvent&)> m_OnMouseLButtonUp = [](Button*, MouseButtonReleasedEvent&) {};
//	std::function<void(Button*, MouseButtonReleasedEvent&)> m_OnClick = [](Button*, MouseButtonReleasedEvent&) {};

	std::unique_ptr<ColorBrush> m_backgroundBrush;
	std::unique_ptr<ColorBrush> m_borderBrush;
	std::unique_ptr<Layout>		m_layout;
	std::array<float, 4>		m_borderWidths;
	D2D1_RECT_F					m_backgroundRect;

	float m_borderTopLeftOffsetX;
	float m_borderTopLeftOffsetY;
	float m_borderTopRightOffsetX;
	float m_borderTopRightOffsetY;
	float m_borderBottomLeftOffsetX;
	float m_borderBottomLeftOffsetY;
	float m_borderBottomRightOffsetX;
	float m_borderBottomRightOffsetY;

	bool m_mouseIsOver;
	bool m_mouseLButtonIsDown;
};
#pragma warning( pop )

}
