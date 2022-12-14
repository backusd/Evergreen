#include "pch.h"
#include "Button.h"

namespace Evergreen
{
Button::Button(std::shared_ptr<DeviceResources> deviceResources,
				const D2D1_RECT_F& allowedRegion,
				std::unique_ptr<ColorBrush> backgroundBrush,
				std::unique_ptr<ColorBrush> borderBrush,
				float borderWidth,
				const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, allowedRegion, margin),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_borderWidth(borderWidth),
	m_backgroundRect({ 0.0f, 0.0f, 1000.0f, 1000.0f }), // dummy values that will be written over when allowed region is updated
	m_mouseIsOver(false), 
	m_mouseLButtonIsDown(false)
{
	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		0.0f, 0.0f, 1000.0f, 1000.0f,
		nullptr, // Don't pass a brush to the layout - Button should draw the background, not the layout
		"button layout");

	ButtonChanged();
}

void Button::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	// Draw the background
	m_deviceResources->D2DDeviceContext()->FillRectangle(m_backgroundRect, m_backgroundBrush->Get());

	// Have the layout draw the background and contents of the brush
	m_layout->Render();

	// Draw the border last so it appears on top
	if (m_borderWidth > 0.0f)
		m_deviceResources->D2DDeviceContext()->DrawRectangle(m_backgroundRect, m_borderBrush->Get(), m_borderWidth);
}

void Button::ButtonChanged()
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// Update the button rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	m_layout->Resize(m_backgroundRect);
}

void Button::OnMarginChanged()
{
	ButtonChanged();
}
void Button::OnAllowedRegionChanged()
{
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");

	// If using a non-SolidColorBrush, we need to update the draw region for the brushes
	m_backgroundBrush->SetDrawRegion(m_allowedRegion);
	m_borderBrush->SetDrawRegion(m_allowedRegion);

	ButtonChanged();
}

bool Button::ContainsPoint(float x, float y) const noexcept
{
	return m_layout->ContainsPoint(x, y);
}

void Button::OnChar(CharEvent& e) noexcept
{
	// Just let layout handle it
	m_layout->OnChar(e);
}
void Button::OnKeyPressed(KeyPressedEvent& e) noexcept
{
	// Just let layout handle it
	m_layout->OnKeyPressed(e);
}
void Button::OnKeyReleased(KeyReleasedEvent& e) noexcept
{
	// Just let layout handle it
	m_layout->OnKeyReleased(e);
}
void Button::OnMouseMove(MouseMoveEvent& e) noexcept
{
	// First pass to layout, if the layout does not handle it, then the button can handle it
	m_layout->OnMouseMove(e);
	if (e.Handled())
		return;

	bool currentMouseIsOver = ContainsPoint(e.GetX(), e.GetY());

	if (m_mouseIsOver)
	{
		// Check if the mouse just left the button area
		if (!currentMouseIsOver)
		{
			m_mouseIsOver = false;
			m_OnMouseExitedButton(this, e);

			// Only set handled=true if the mouse is down, making it so that another control can not process this event
			// The button should continue to handle mouse events until the mouse button is released
			if (m_mouseLButtonIsDown)
				e.Handled(this);

			return; // Need this return because it is invalid to set e.Handled() more than once
		}

		// Nothing changed here, just make sure the event is handled and return
		m_OnMouseMoved(this, e);
		e.Handled(this);
	}
	else if (currentMouseIsOver) // Check if the mouse is newly over the button
	{
		m_mouseIsOver = true;
		m_OnMouseEnteredButton(this, e);
		e.Handled(this);
	}
	else if (m_mouseLButtonIsDown)
	{
		// If the mouse is outside the button but the LButton was pressed while the mouse was
		// over the button, we still want the button to handle the move event until the mouse 
		// LButton is released
		e.Handled(this);
	}
}
void Button::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	// First pass to layout, if the layout does not handle it, then the button can handle it
	m_layout->OnMouseButtonPressed(e);
	if (e.Handled())
		return;

	// Only process the event if the mouse is over the button and if its the LButton
	if (m_mouseIsOver)
	{
		// As long as the mouse is still over the button, we want to consider the event to be handled,
		// even if we don't actually do anything
		e.Handled(this);

		if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
		{
			m_mouseLButtonIsDown = true;
			m_OnMouseLButtonDown(this, e);
		}
	}
}
void Button::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	// First pass to layout, if the layout does not handle it, then the button can handle it
	m_layout->OnMouseButtonReleased(e);
	if (e.Handled())
		return;

	// Only process the event if the mouse is over the button and if its the LButton
	if (m_mouseIsOver)
	{
		// As long as the mouse is still over the button, we want to consider the event to be handled,
		// even if we don't actually do anything
		e.Handled(this);

		if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
		{
			m_mouseLButtonIsDown = false;
			m_OnMouseLButtonUp(this, e);
			m_OnClick(this, e);
		}
	}
	else
	{
		// It is possible to get this event when the mouse LButton had been clicked down when the mouse
		// was over the button, but got moved off the button before the LButton was released. In this case,
		// we need to make sure that we clear the m_mouseLButtonIsDown so that mouse move events don't continue
		// to be handled by this button
		m_mouseLButtonIsDown = false;
	}
}
void Button::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{
	// Just let layout handle it
	m_layout->OnMouseButtonDoubleClick(e);
}
void Button::OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept
{
	// Just let layout handle it
	m_layout->OnMouseScrolledVertical(e);
}
void Button::OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept
{
	// Just let layout handle it
	m_layout->OnMouseScrolledHorizontal(e);
}

}