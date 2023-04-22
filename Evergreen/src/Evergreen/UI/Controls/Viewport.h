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
	Viewport& operator=(const Viewport&) noexcept = delete;
	virtual ~Viewport() noexcept override {}

	// Event Handlers
	virtual void HandleOnChar(CharEvent&) {}
	virtual void HandleOnKeyPressed(KeyPressedEvent&) {}
	virtual void HandleOnKeyReleased(KeyReleasedEvent&) {}
	virtual void HandleOnMouseEntered(MouseMoveEvent&) {}
	virtual void HandleOnMouseExited(MouseMoveEvent&) {}
	virtual void HandleOnMouseMove(MouseMoveEvent&) {}
	virtual void HandleOnMouseScrolledVertical(MouseScrolledEvent&) {}
	virtual void HandleOnMouseScrolledHorizontal(MouseScrolledEvent&) {}
	virtual void HandleOnMouseButtonPressed(MouseButtonPressedEvent&) {}
	virtual void HandleOnMouseButtonReleased(MouseButtonReleasedEvent&) {}
	virtual void HandleOnClick(MouseButtonReleasedEvent&) {}
	virtual void HandleOnDoubleClick(MouseButtonDoubleClickEvent&) {}
	virtual void HandleOnSizeChanged(float width, float height) {}

	// Inherited from Control
	void OnUpdate(const Timer& timer) override;
	void Render() const override;

	// There is a special case where a Pane has handled a move event, but this Control was previously handling
	// the move events. Therefore, we need to inform this Control that the mouse is no longer over the Control
	void MouseMoveHandledByPane(MouseMoveEvent& e) override;

	ND inline virtual Layout* GetLayoutByName(const std::string& name) noexcept override { return m_layout->GetLayoutByName(name); }
	ND inline virtual Layout* GetLayoutByID(unsigned int id) noexcept override { return m_layout->GetLayoutByID(id); }
	ND virtual Control* GetControlByName(const std::string& name) noexcept override;
	ND virtual Control* GetControlByID(unsigned int id) noexcept override;

	// GET
	ND inline float GetAspectRatio() const noexcept { return m_viewport.Width / m_viewport.Height; }
	ND inline const D3D11_VIEWPORT& GetViewport() const { return m_viewport; }
	ND inline Layout* GetLayout() const noexcept { return m_layout.get(); }

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

	virtual ControlType GetControlType() const noexcept override { return ControlType::Viewport; }

protected:

	void ViewportChanged();

	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;

	ND inline bool ContainsPoint(float x, float y) const noexcept;

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
