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

	ND inline virtual Layout* GetLayoutByName(const std::string& name) noexcept override { return m_layout->GetLayoutByName(name); }
	ND inline virtual Layout* GetLayoutByID(unsigned int id) noexcept override { return m_layout->GetLayoutByID(id); }
	ND virtual Control* GetControlByName(const std::string& name) noexcept override;
	ND virtual Control* GetControlByID(unsigned int id) noexcept override;

	// GET
	ND inline float GetAspectRatio() const noexcept { return m_viewport.Width / m_viewport.Height; }
	ND inline const D3D11_VIEWPORT& GetViewport() const { return m_viewport; }
	ND inline Layout* GetLayout() const noexcept { return m_layout.get(); }

	// SET
	inline void SetOnCharCallback(std::function<void(Viewport*, CharEvent& e)> func) { m_OnChar = func; }
	inline void SetOnKeyPressedCallback(std::function<void(Viewport*, KeyPressedEvent& e)> func) { m_OnKeyPressed = func; }
	inline void SetOnKeyReleasedCallback(std::function<void(Viewport*, KeyReleasedEvent& e)> func) { m_OnKeyReleased = func; }
	inline void SetOnMouseEnteredCallback(std::function<void(Viewport*, MouseMoveEvent& e)> func) { m_OnMouseEntered = func; }
	inline void SetOnMouseExitedCallback(std::function<void(Viewport*, MouseMoveEvent& e)> func) { m_OnMouseExited = func; }
	inline void SetOnMouseMovedCallback(std::function<void(Viewport*, MouseMoveEvent& e)> func) { m_OnMouseMoved = func; }
	inline void SetOnMouseScrolledVerticalCallback(std::function<void(Viewport*, MouseScrolledEvent& e)> func) { m_OnMouseScrolledVertical = func; }
	inline void SetOnMouseScrolledHorizontalCallback(std::function<void(Viewport*, MouseScrolledEvent& e)> func) { m_OnMouseScrolledHorizontal = func; }
	inline void SetOnMouseButtonPressedCallback(std::function<void(Viewport*, MouseButtonPressedEvent& e)> func) { m_OnMouseButtonPressed = func; }
	inline void SetOnMouseButtonReleasedCallback(std::function<void(Viewport*, MouseButtonReleasedEvent& e)> func) { m_OnMouseButtonReleased = func; }
	inline void SetOnClickCallback(std::function<void(Viewport*, MouseButtonReleasedEvent& e)> func) { m_OnClick = func; }
	inline void SetOnDoubleClickCallback(std::function<void(Viewport*, MouseButtonDoubleClickEvent& e)> func) { m_OnDoubleClick = func; }

	inline void SetOnSizeChangedCallback(std::function<void(float, float)> fn) noexcept { m_OnSizeChanged = fn; }

	// Event handling
	void OnChar(CharEvent& e) override;
	void OnKeyPressed(KeyPressedEvent& e) override;
	void OnKeyReleased(KeyReleasedEvent& e) override;
	void OnMouseMove(MouseMoveEvent& e) override;
	void OnMouseScrolledVertical(MouseScrolledEvent& e) override;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) override;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) override;

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

	std::function<void(float, float)> m_OnSizeChanged = [](float width, float height) {};

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
