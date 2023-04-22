#include "pch.h"
#include "Viewport.h"


namespace Evergreen
{
Viewport::Viewport(std::shared_ptr<DeviceResources> deviceResources, UI* ui, const D2D1_RECT_F& allowedRegion, const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, ui, allowedRegion, margin),
	m_viewport(CD3D11_VIEWPORT(
		m_allowedRegion.left + m_margin.Left,
		m_allowedRegion.top + m_margin.Top,
		m_allowedRegion.right - m_allowedRegion.left - m_margin.Right - m_margin.Left,
		m_allowedRegion.bottom - m_allowedRegion.top - m_margin.Bottom - m_margin.Top
	)),
	m_layout(nullptr),
	m_selected(false),
	m_mouseIsOver(false),
	m_mouseLButtonDown(false),
	m_mouseMButtonDown(false),
	m_mouseRButtonDown(false),
	m_mouseX1ButtonDown(false),
	m_mouseX2ButtonDown(false)
{
	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		ui,
		m_viewport.TopLeftY,
		m_viewport.TopLeftX,
		m_viewport.Width,
		m_viewport.Height,
		nullptr, // Don't pass a brush to the layout
		"viewport layout"
	);
}

void Viewport::ViewportChanged()
{
	m_viewport = CD3D11_VIEWPORT(
		m_allowedRegion.left + m_margin.Left, 
		m_allowedRegion.top + m_margin.Top, 
		m_allowedRegion.right - m_allowedRegion.left - m_margin.Right - m_margin.Left,
		m_allowedRegion.bottom - m_allowedRegion.top - m_margin.Bottom - m_margin.Top
	);

	m_layout->Resize(
		m_viewport.TopLeftY,
		m_viewport.TopLeftX,
		m_viewport.Width,
		m_viewport.Height
	);

	HandleOnSizeChanged(m_viewport.Width, m_viewport.Height);
}
void Viewport::OnMarginChanged()
{
	ViewportChanged();
}
void Viewport::OnAllowedRegionChanged()
{
	ViewportChanged();
}

void Viewport::OnUpdate(const Timer& timer)
{
	EG_CORE_ASSERT(m_layout != nullptr, "Layout is nullptr");
	m_layout->Update(timer);
}
void Viewport::Render() const
{
	EG_CORE_ASSERT(m_layout != nullptr, "Layout is nullptr");
	m_layout->Render();
}

void Viewport::OnChar(CharEvent& e)
{
	// See if the layout wants to handle the event
	m_layout->OnChar(e);
	if (e.Handled())
		return;

	if (m_selected)
	{
		OnChar(e);
		e.Handled(this);
	}
}
void Viewport::OnKeyPressed(KeyPressedEvent& e)
{
	// See if the layout wants to handle the event
	m_layout->OnKeyPressed(e);
	if (e.Handled())
		return;

	if (m_selected)
	{
		OnKeyPressed(e);
		e.Handled(this);
	}
}
void Viewport::OnKeyReleased(KeyReleasedEvent& e)
{
	// See if the layout wants to handle the event
	m_layout->OnKeyReleased(e);
	if (e.Handled())
		return;

	if (m_selected)
	{
		OnKeyReleased(e);
		e.Handled(this);
	}
}
void Viewport::OnMouseMove(MouseMoveEvent& e)
{
	// See if the layout wants to handle the event
	m_layout->OnMouseMove(e);
	if (e.Handled())
		return;

	bool currentMouseIsOver = ContainsPoint(e.GetX(), e.GetY());

	if (m_mouseIsOver)
	{
		if (!currentMouseIsOver)
		{
			m_mouseIsOver = false;
			HandleOnMouseExited(e);

			// Continue to track the move events if a button is down
			if (m_mouseLButtonDown || m_mouseMButtonDown || m_mouseRButtonDown || m_mouseX1ButtonDown || m_mouseX2ButtonDown)
			{
				HandleOnMouseMove(e);
				e.Handled(this);
			}
		}
		else 
		{
			HandleOnMouseMove(e);
			e.Handled(this);
		}
	}
	else if (currentMouseIsOver)
	{
		// NOTE: Don't set m_selected = true because we only want it to be selected after it has been clicked
		m_mouseIsOver = true;
		HandleOnMouseEntered(e);
		e.Handled(this);
	}
	else if (m_selected)
	{
		// The viewport is "selected" meaning that we want to continue to handle keyboard events
		// (this the viewport will become unselected when the user clicks on another control)
		if (m_mouseLButtonDown || m_mouseMButtonDown || m_mouseRButtonDown || m_mouseX1ButtonDown || m_mouseX2ButtonDown)
		{
			HandleOnMouseMove(e);
		}
		e.Handled(this);
	}
}
void Viewport::MouseMoveHandledByPane(MouseMoveEvent& e)
{
	m_mouseIsOver = false;
	HandleOnMouseExited(e);
}
void Viewport::OnMouseScrolledVertical(MouseScrolledEvent& e)
{
	// See if the layout wants to handle the event
	m_layout->OnMouseScrolledVertical(e);
	if (e.Handled())
		return;

	if (m_mouseIsOver)
	{
		HandleOnMouseScrolledVertical(e);
		e.Handled(this);
	}
}
void Viewport::OnMouseScrolledHorizontal(MouseScrolledEvent& e)
{
	// See if the layout wants to handle the event
	m_layout->OnMouseScrolledVertical(e);
	if (e.Handled())
		return;

	if (m_mouseIsOver)
	{
		HandleOnMouseScrolledHorizontal(e);
		e.Handled(this);
	}
}
void Viewport::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	if (m_mouseIsOver)
	{
		m_layout->OnMouseButtonPressed(e);
		if (e.Handled())
			return;

		switch (e.GetMouseButton())
		{
		case Evergreen::MOUSE_BUTTON::EG_LBUTTON: m_mouseLButtonDown = true; break;
		case Evergreen::MOUSE_BUTTON::EG_MBUTTON: m_mouseMButtonDown = true; break;
		case Evergreen::MOUSE_BUTTON::EG_RBUTTON: m_mouseRButtonDown = true; break;
		case Evergreen::MOUSE_BUTTON::EG_XBUTTON1: m_mouseX1ButtonDown = true; break;
		case Evergreen::MOUSE_BUTTON::EG_XBUTTON2: m_mouseX2ButtonDown = true; break;
		}

		m_selected = true;
		HandleOnMouseButtonPressed(e);
		e.Handled(this);
	}
	else
	{
		m_selected = false;
	}
}
void Viewport::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	// Basic idea here is that we should only handle the Release event if we had previously registered
	// the corresponding Pressed event

	switch (e.GetMouseButton())
	{
	case Evergreen::MOUSE_BUTTON::EG_LBUTTON: 
		if (m_mouseLButtonDown)
		{
			m_mouseLButtonDown = false;
			HandleOnMouseButtonReleased(e);
			HandleOnClick(e);
			e.Handled(this);
		}
		else
		{
			m_layout->OnMouseButtonReleased(e);
		}
		break;
	case Evergreen::MOUSE_BUTTON::EG_MBUTTON: 
		if (m_mouseMButtonDown)
		{
			m_mouseMButtonDown = false;
			HandleOnMouseButtonReleased(e);
			HandleOnClick(e);
			e.Handled(this);
		}
		else
		{
			m_layout->OnMouseButtonReleased(e);
		}
		break;
	case Evergreen::MOUSE_BUTTON::EG_RBUTTON: 
		if (m_mouseRButtonDown)
		{
			m_mouseRButtonDown = false;
			HandleOnMouseButtonReleased(e);
			HandleOnClick(e);
			e.Handled(this);
		}
		else
		{
			m_layout->OnMouseButtonReleased(e);
		}
		break;
	case Evergreen::MOUSE_BUTTON::EG_XBUTTON1: 
		if (m_mouseX1ButtonDown)
		{
			m_mouseX1ButtonDown = false;
			HandleOnMouseButtonReleased(e);
			HandleOnClick(e);
			e.Handled(this);
		}
		else
		{
			m_layout->OnMouseButtonReleased(e);
		}
		break;
	case Evergreen::MOUSE_BUTTON::EG_XBUTTON2: 
		if (m_mouseX2ButtonDown)
		{
			m_mouseX2ButtonDown = false;
			HandleOnMouseButtonReleased(e);
			HandleOnClick(e);
			e.Handled(this);
		}
		else
		{
			m_layout->OnMouseButtonReleased(e);
		}
		break;
	}
}
void Viewport::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e)
{
	// See if the layout wants to handle the event
	m_layout->OnMouseButtonDoubleClick(e);
	if (e.Handled())
		return;

	if (m_mouseIsOver)
	{
		HandleOnDoubleClick(e);
		e.Handled(this);
	}
}

bool Viewport::ContainsPoint(float x, float y) const noexcept
{
	return m_viewport.TopLeftX <= x &&
		m_viewport.TopLeftX + m_viewport.Width >= x &&
		m_viewport.TopLeftY <= y&&
		m_viewport.TopLeftY + m_viewport.Height >= y;
}

Control* Viewport::GetControlByName(const std::string& name) noexcept
{
	if (m_name.compare(name) == 0)
		return this;

	return m_layout->GetControlByName(name);
}
Control* Viewport::GetControlByID(unsigned int id) noexcept
{
	if (m_id == id)
		return this;

	return m_layout->GetControlByID(id);
}

}