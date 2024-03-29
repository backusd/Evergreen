#include "pch.h"
#include "Button.h"
#include "Text.h"

namespace Evergreen
{
Button::Button(std::shared_ptr<DeviceResources> deviceResources,
				UI* ui,
				const D2D1_RECT_F& allowedRegion,
				std::unique_ptr<ColorBrush> backgroundBrush,
				std::unique_ptr<ColorBrush> borderBrush,
				const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, ui, allowedRegion, margin),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_backgroundRect({ 0.0f, 0.0f, 1000.0f, 1000.0f }), // dummy values that will be written over when allowed region is updated
	m_mouseIsOver(false), 
	m_mouseLButtonIsDown(false),
	m_borderWidths{ 0.0f, 0.0f, 0.0f, 0.0f },
	m_borderTopLeftOffsetX(0.0f),
	m_borderTopLeftOffsetY(0.0f),
	m_borderTopRightOffsetX(0.0f),
	m_borderTopRightOffsetY(0.0f),
	m_borderBottomLeftOffsetX(0.0f),
	m_borderBottomLeftOffsetY(0.0f),
	m_borderBottomRightOffsetX(0.0f),
	m_borderBottomRightOffsetY(0.0f),
	m_cornerRadiusX(0.0f),
	m_cornerRadiusY(0.0f),
	m_roundedRect{},
	m_roundedRectGeometry(nullptr)
{
	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		ui,
		0.0f, 0.0f, 1000.0f, 1000.0f,
		nullptr, // Don't pass a brush to the layout - Button should draw the background, not the layout
		"button layout");

	ButtonChanged();
}
void Button::OnUpdate(const Timer& timer)
{
	m_layout->Update(timer);
}

void Button::Render() const
{
	if (m_cornerRadiusX > 0.0f && m_cornerRadiusY > 0.0f)
		RenderRoundedRect();
	else
		RenderRect();
}
void Button::RenderRect() const
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	auto context = m_deviceResources->D2DDeviceContext();

	// Draw the background
	context->FillRectangle(m_backgroundRect, m_backgroundBrush->Get());

	// Have the layout draw the background and contents of the brush
	m_layout->Render();

	// Draw the border last so it appears on top
	float left = m_backgroundRect.left;
	float right = m_backgroundRect.right;
	float top = m_backgroundRect.top;
	float bottom = m_backgroundRect.bottom;

	if (m_borderWidths[0] > 0.0f)
	{
		// If the offsets are 0, then just draw the complete border
		if (m_borderTopLeftOffsetY == 0.0f && m_borderBottomLeftOffsetY == 0.0f)
		{
			context->DrawLine(
				D2D1::Point2F(left, top),		// top-left
				D2D1::Point2F(left, bottom),	// bottom-left
				m_borderBrush->Get(),
				m_borderWidths[0]
			);
		}
		else
		{
			context->DrawLine(
				D2D1::Point2F(left, top),							// top-left
				D2D1::Point2F(left, top + m_borderTopLeftOffsetY),	// stop point from the top
				m_borderBrush->Get(),
				m_borderWidths[0]
			);
			context->DrawLine(
				D2D1::Point2F(left, bottom),								// bottom-left
				D2D1::Point2F(left, bottom - m_borderBottomLeftOffsetY),	// stop point from the bottom
				m_borderBrush->Get(),
				m_borderWidths[0]
			);
		}
	}
	if (m_borderWidths[1] > 0.0f)
	{
		// If the offsets are 0, then just draw the complete border
		if (m_borderTopLeftOffsetX == 0.0f && m_borderTopRightOffsetX == 0.0f)
		{
			context->DrawLine(
				D2D1::Point2F(left, top),	// top-left
				D2D1::Point2F(right, top),	// top-right
				m_borderBrush->Get(),
				m_borderWidths[1]
			);
		}
		else
		{
			context->DrawLine(
				D2D1::Point2F(left, top),							// top-left
				D2D1::Point2F(left + m_borderTopLeftOffsetX, top),	// stop point from the left
				m_borderBrush->Get(),
				m_borderWidths[1]
			);
			context->DrawLine(
				D2D1::Point2F(right, top),								// top-right
				D2D1::Point2F(right - m_borderTopRightOffsetX, top),	// stop point from the right
				m_borderBrush->Get(),
				m_borderWidths[1]
			);
		}
	}
	if (m_borderWidths[2] > 0.0f)
	{
		// If the offsets are 0, then just draw the complete border
		if (m_borderTopRightOffsetY == 0.0f && m_borderBottomRightOffsetY == 0.0f)
		{
			context->DrawLine(
				D2D1::Point2F(right, top),		// top-right
				D2D1::Point2F(right, bottom),	// bottom-right
				m_borderBrush->Get(),
				m_borderWidths[2]
			);
		}
		else
		{
			context->DrawLine(
				D2D1::Point2F(right, top),								// top-right
				D2D1::Point2F(right, top + m_borderTopRightOffsetY),	// stop point from the top
				m_borderBrush->Get(),
				m_borderWidths[2]
			);
			context->DrawLine(
				D2D1::Point2F(right, bottom),								// bottom-right
				D2D1::Point2F(right, bottom - m_borderBottomRightOffsetY),	// stop point from the bottom
				m_borderBrush->Get(),
				m_borderWidths[2]
			);
		}
	}
	if (m_borderWidths[3] > 0.0f)
	{
		// If the offsets are 0, then just draw the complete border
		if (m_borderBottomLeftOffsetX == 0.0f && m_borderBottomRightOffsetX == 0.0f)
		{
			context->DrawLine(
				D2D1::Point2F(left, bottom),	// bottom-left
				D2D1::Point2F(right, bottom),	// bottom-right
				m_borderBrush->Get(),
				m_borderWidths[3]
			);
		}
		else
		{
			context->DrawLine(
				D2D1::Point2F(left, bottom),								// bottom-left
				D2D1::Point2F(left + m_borderBottomLeftOffsetX, bottom),	// stop point from the left
				m_borderBrush->Get(),
				m_borderWidths[3]
			);
			context->DrawLine(
				D2D1::Point2F(right, bottom),								// bottom-right
				D2D1::Point2F(right - m_borderBottomRightOffsetX, bottom),	// stop point from the right
				m_borderBrush->Get(),
				m_borderWidths[3]
			);
		}
	}
}
void Button::RenderRoundedRect() const
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	auto context = m_deviceResources->D2DDeviceContext();

	context->FillRoundedRectangle(m_roundedRect, m_backgroundBrush->Get());

	m_layout->Render();

	// Draw the border last so it appears on top
	// NOTE: We do NOT support drawing partial borders when drawing a rounded rect, so just use the first border width in the array
	if (m_borderWidths[0] > 0.0f)
		context->DrawRoundedRectangle(m_roundedRect, m_borderBrush->Get(), m_borderWidths[0]);
}

void Button::ButtonChanged()
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// Update the button rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	if (m_cornerRadiusX > 0.0f && m_cornerRadiusY > 0.0f)
	{
		m_roundedRect = D2D1::RoundedRect(m_backgroundRect, m_cornerRadiusX, m_cornerRadiusY);

		// Create the rounded rect geometry (used for determining if mouse is over button)
		GFX_THROW_INFO(
			m_deviceResources->D2DFactory()->CreateRoundedRectangleGeometry(m_roundedRect, m_roundedRectGeometry.ReleaseAndGetAddressOf())
		)
	}

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
	if (m_cornerRadiusX > 0.0f && m_cornerRadiusY > 0.0f)
	{
		EG_CORE_ASSERT(m_roundedRectGeometry != nullptr, "No rounded rect geometry");
		BOOL b;
		m_roundedRectGeometry->FillContainsPoint(D2D1::Point2F(x, y), D2D1::Matrix3x2F::Identity(), &b);
		return static_cast<bool>(b);
	}

	return m_layout->ContainsPoint(x, y);
}

void Button::OnChar(CharEvent& e)
{
	// Just let layout handle it
	m_layout->OnChar(e);
}
void Button::OnKeyPressed(KeyPressedEvent& e)
{
	// Just let layout handle it
	m_layout->OnKeyPressed(e);
}
void Button::OnKeyReleased(KeyReleasedEvent& e)
{
	// Just let layout handle it
	m_layout->OnKeyReleased(e);
}
void Button::OnMouseMove(MouseMoveEvent& e)
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
			m_OnMouseExited(this, e);

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
		m_OnMouseEntered(this, e);
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
void Button::MouseMoveHandledByPane(MouseMoveEvent& e)
{
	m_mouseIsOver = false;
	m_OnMouseExited(this, e);
}
void Button::OnMouseButtonPressed(MouseButtonPressedEvent& e)
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
void Button::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
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
void Button::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e)
{
	// Just let layout handle it
	m_layout->OnMouseButtonDoubleClick(e);
}
void Button::OnMouseScrolledVertical(MouseScrolledEvent& e)
{
	// Just let layout handle it
	m_layout->OnMouseScrolledVertical(e);
}
void Button::OnMouseScrolledHorizontal(MouseScrolledEvent& e)
{
	// Just let layout handle it
	m_layout->OnMouseScrolledHorizontal(e);
}

Control* Button::GetControlByName(const std::string& name) noexcept
{
	if (m_name.compare(name) == 0)
		return this;

	return m_layout->GetControlByName(name);
}
Control* Button::GetControlByID(unsigned int id) noexcept
{
	if (m_id == id)
		return this;

	return m_layout->GetControlByID(id);
}

void Button::BackgroundBrush(const D2D1_COLOR_F& color)
{
	m_backgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, color);
}
void Button::BackgroundBrush(D2D1::ColorF::Enum color)
{
	m_backgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(color));
}
void Button::BackgroundBrushAndTextColor(const D2D1_COLOR_F& buttonColor, const D2D1_COLOR_F& textColor)
{
	BackgroundBrush(buttonColor);

	Text* text = static_cast<Text*>(m_layout->GetFirstControlOfType(Control::ControlType::Text));
	EG_ASSERT(text != nullptr, "Should not be calling this function if the button has no Text child control");
	text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, textColor)));
}
void Button::BackgroundBrushAndTextColor(D2D1::ColorF::Enum buttonColor, const D2D1_COLOR_F& textColor)
{
	BackgroundBrush(buttonColor);

	Text* text = static_cast<Text*>(m_layout->GetFirstControlOfType(Control::ControlType::Text));
	EG_ASSERT(text != nullptr, "Should not be calling this function if the button has no Text child control");
	text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, textColor)));
}
void Button::BackgroundBrushAndTextColor(const D2D1_COLOR_F& buttonColor, D2D1::ColorF::Enum textColor)
{
	BackgroundBrush(buttonColor);

	Text* text = static_cast<Text*>(m_layout->GetFirstControlOfType(Control::ControlType::Text));
	EG_ASSERT(text != nullptr, "Should not be calling this function if the button has no Text child control");
	text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(textColor))));
}
void Button::BackgroundBrushAndTextColor(D2D1::ColorF::Enum buttonColor, D2D1::ColorF::Enum textColor)
{
	BackgroundBrush(buttonColor);

	Text* text = static_cast<Text*>(m_layout->GetFirstControlOfType(Control::ControlType::Text));
	EG_ASSERT(text != nullptr, "Should not be calling this function if the button has no Text child control");
	text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(textColor))));
}


}