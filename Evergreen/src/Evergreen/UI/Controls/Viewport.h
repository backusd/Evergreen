#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/UI/Layout.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Viewport : public Control
{
public:
	Viewport(std::shared_ptr<DeviceResources> deviceResources,
				UI* ui,
				const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
				const Evergreen::Margin& margin = { 0 }) noexcept;
	Viewport(const Viewport&) noexcept = delete; // Just delete for now until there is a good use case
	void operator=(const Viewport&) noexcept = delete;
	virtual ~Viewport() noexcept override {}

	// Inherited from Control
	void OnUpdate(const Timer& timer) override;
	void Render() const override;

	ND virtual Control* GetControlByName(const std::string& name) noexcept override;
	ND virtual Control* GetControlByID(unsigned int id) noexcept override;

	// GET
	ND inline const D3D11_VIEWPORT& GetViewport() const { return m_viewport; }
	ND inline Layout* GetLayout() const noexcept { return m_layout.get(); }

	// SET
	inline void SetOnCharCallback(std::function<void(Viewport*, CharEvent& e)> func) noexcept { m_OnChar = func; }
	inline void SetOnKeyPressedCallback(std::function<void(Viewport*, KeyPressedEvent& e)> func) noexcept { m_OnKeyPressed = func; }
	inline void SetOnKeyReleasedCallback(std::function<void(Viewport*, KeyReleasedEvent& e)> func) noexcept { m_OnKeyReleased = func; }
	inline void SetOnMouseEnteredCallback(std::function<void(Viewport*, MouseMoveEvent& e)> func) noexcept { m_OnMouseEntered = func; }
	inline void SetOnMouseExitedCallback(std::function<void(Viewport*, MouseMoveEvent& e)> func) noexcept { m_OnMouseExited = func; }
	inline void SetOnMouseMovedCallback(std::function<void(Viewport*, MouseMoveEvent& e)> func) noexcept { m_OnMouseMoved = func; }
	inline void SetOnMouseScrolledVerticalCallback(std::function<void(Viewport*, MouseScrolledEvent& e)> func) noexcept { m_OnMouseScrolledVertical = func; }
	inline void SetOnMouseScrolledHorizontalCallback(std::function<void(Viewport*, MouseScrolledEvent& e)> func) noexcept { m_OnMouseScrolledHorizontal = func; }
	inline void SetOnMouseButtonPressedCallback(std::function<void(Viewport*, MouseButtonPressedEvent& e)> func) noexcept { m_OnMouseButtonPressed = func; }
	inline void SetOnMouseButtonReleasedCallback(std::function<void(Viewport*, MouseButtonReleasedEvent& e)> func) noexcept { m_OnMouseButtonReleased = func; }
	inline void SetOnClickCallback(std::function<void(Viewport*, MouseButtonReleasedEvent& e)> func) noexcept { m_OnClick = func; }
	inline void SetOnDoubleClickCallback(std::function<void(Viewport*, MouseButtonDoubleClickEvent& e)> func) noexcept { m_OnDoubleClick = func; }

	// Event handling
	void OnChar(CharEvent& e) noexcept override;
	void OnKeyPressed(KeyPressedEvent& e) noexcept override;
	void OnKeyReleased(KeyReleasedEvent& e) noexcept override;
	void OnMouseMove(MouseMoveEvent& e) noexcept override;
	void OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept override;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept override;

protected:

	void ViewportChanged();

	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;

	ND inline bool ContainsPoint(float x, float y) const noexcept;

	std::function<void(Viewport*, CharEvent&)> m_OnChar = [](Viewport*, CharEvent&) {};
	std::function<void(Viewport*, KeyPressedEvent&)> m_OnKeyPressed = [](Viewport*, KeyPressedEvent&) {};
	std::function<void(Viewport*, KeyReleasedEvent&)> m_OnKeyReleased = [](Viewport*, KeyReleasedEvent&) {};
	std::function<void(Viewport*, MouseMoveEvent&)> m_OnMouseEntered = [](Viewport*, MouseMoveEvent&) {};
	std::function<void(Viewport*, MouseMoveEvent&)> m_OnMouseExited = [](Viewport*, MouseMoveEvent&) {};
	std::function<void(Viewport*, MouseMoveEvent&)> m_OnMouseMoved = [](Viewport*, MouseMoveEvent&) {};
	std::function<void(Viewport*, MouseScrolledEvent&)> m_OnMouseScrolledVertical = [](Viewport*, MouseScrolledEvent&) {};
	std::function<void(Viewport*, MouseScrolledEvent&)> m_OnMouseScrolledHorizontal = [](Viewport*, MouseScrolledEvent&) {};
	std::function<void(Viewport*, MouseButtonPressedEvent&)> m_OnMouseButtonPressed = [](Viewport*, MouseButtonPressedEvent&) {};
	std::function<void(Viewport*, MouseButtonReleasedEvent&)> m_OnMouseButtonReleased = [](Viewport*, MouseButtonReleasedEvent&) {};
	std::function<void(Viewport*, MouseButtonReleasedEvent&)> m_OnClick = [](Viewport*, MouseButtonReleasedEvent&) {};
	std::function<void(Viewport*, MouseButtonDoubleClickEvent&)> m_OnDoubleClick = [](Viewport*, MouseButtonDoubleClickEvent&) {};

	D3D11_VIEWPORT m_viewport;
	std::unique_ptr<Layout>	m_layout;

	bool m_selected;
	bool m_mouseIsOver;

	bool m_mouseLButtonDown;
	bool m_mouseMButtonDown;
	bool m_mouseRButtonDown;
	bool m_mouseX1ButtonDown;
	bool m_mouseX2ButtonDown;
};
#pragma warning( pop )

}
