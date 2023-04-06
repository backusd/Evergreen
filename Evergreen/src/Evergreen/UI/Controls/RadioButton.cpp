#include "pch.h"
#include "RadioButton.h"



namespace Evergreen
{
RadioButton::RadioButton(std::shared_ptr<DeviceResources> deviceResources,
						UI* ui,
						const D2D1_RECT_F& allowedRegion,
						bool isChecked,
						float outerRadius,
						float innerRadius,
						std::unique_ptr<ColorBrush> outerBrush,
						std::unique_ptr<ColorBrush> innerBrush,
						const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, ui, allowedRegion, margin),
	m_isChecked(isChecked),
	m_innerRadius(innerRadius),
	m_outerRadius(outerRadius),
	m_outerBrush(std::move(outerBrush)),
	m_innerBrush(std::move(innerBrush)),
	m_outerLineWidth(1.5f),
	m_innerCircle(nullptr),
	m_outerCircle(nullptr),
	m_mouseState(MouseOverState::NOT_OVER)
{
	if (m_innerBrush == nullptr)
		m_innerBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue));

	if (m_outerBrush == nullptr)
		m_outerBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue));

	RadioButtonChanged();
}

void RadioButton::Render() const
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_innerBrush != nullptr, "No inner brush");
	EG_CORE_ASSERT(m_outerBrush != nullptr, "No outer brush");
	EG_CORE_ASSERT(m_innerCircle != nullptr, "No inner circle");
	EG_CORE_ASSERT(m_outerCircle != nullptr, "No outer circle");

	auto context = m_deviceResources->D2DDeviceContext();

	D2D1_ELLIPSE outerCircle;
	m_outerCircle->GetEllipse(&outerCircle);
	context->DrawEllipse(outerCircle, m_outerBrush->Get(), m_outerLineWidth);

	if (m_isChecked)
	{
		D2D1_ELLIPSE innerCircle;
		m_innerCircle->GetEllipse(&innerCircle);
		context->FillEllipse(innerCircle, m_innerBrush->Get());
	}
}

void RadioButton::RadioButtonChanged()
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	float left = m_allowedRegion.left + m_margin.Left;
	float top = m_allowedRegion.top + m_margin.Top;
	float right = m_allowedRegion.right + m_margin.Right;
	float bottom = m_allowedRegion.bottom + m_margin.Bottom;
	D2D1_POINT_2F center = D2D1::Point2F(
		left + ((right - left) / 2),
		top + ((bottom - top) / 2)
	);
	D2D1_ELLIPSE innerCircle = D2D1::Ellipse(center, m_innerRadius, m_innerRadius);
	D2D1_ELLIPSE outerCircle = D2D1::Ellipse(center, m_outerRadius, m_outerRadius);

	GFX_THROW_INFO(
		m_deviceResources->D2DFactory()->CreateEllipseGeometry(innerCircle, m_innerCircle.ReleaseAndGetAddressOf())
	)
	GFX_THROW_INFO(
		m_deviceResources->D2DFactory()->CreateEllipseGeometry(outerCircle, m_outerCircle.ReleaseAndGetAddressOf())
	)
}
void RadioButton::OnAllowedRegionChanged()
{
	EG_CORE_ASSERT(m_innerBrush != nullptr, "No inner brush");
	EG_CORE_ASSERT(m_outerBrush != nullptr, "No outer brush");

	// If using a non-SolidColorBrush, we need to update the draw region for the brushes
	m_innerBrush->SetDrawRegion(m_allowedRegion);
	m_outerBrush->SetDrawRegion(m_allowedRegion);

	RadioButtonChanged();
}

void RadioButton::SetIsChecked(bool checked) noexcept
{
	if (m_isChecked != checked)
	{
		m_isChecked = checked;
		RadioButtonIsCheckedChangedEvent e(checked);
		m_OnIsCheckedChanged(this, e);
	}
}

// Event handling
void RadioButton::OnMouseMove(MouseMoveEvent& e)
{
	bool currentMouseIsOver = MouseIsOver(e.GetX(), e.GetY());

	switch (m_mouseState)
	{
	case MouseOverState::NOT_OVER:
	{
		if (currentMouseIsOver)
		{
			m_mouseState = MouseOverState::OVER;
			m_OnMouseEntered(this, e);
			e.Handled(this);
		}
		break;
	}
	case MouseOverState::OVER:
	case MouseOverState::OVER_AND_LBUTTON_DOWN:
	{
		m_OnMouseMoved(this, e);
		if (!currentMouseIsOver)
		{
			m_mouseState = MouseOverState::NOT_OVER;
			m_OnMouseExited(this, e);			
		}
		e.Handled(this);
		break;
	}
	}
}
void RadioButton::MouseMoveHandledByPane(MouseMoveEvent& e)
{
	m_mouseState = MouseOverState::NOT_OVER;
	m_OnMouseExited(this, e);
}
void RadioButton::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON &&
		m_mouseState == MouseOverState::OVER)
	{
		m_mouseState = MouseOverState::OVER_AND_LBUTTON_DOWN;
		m_OnMouseLButtonDown(this, e);
		e.Handled(this);
	}
}
void RadioButton::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON &&
		m_mouseState == MouseOverState::OVER_AND_LBUTTON_DOWN)
	{
		m_isChecked = !m_isChecked;
		m_mouseState = MouseOverState::OVER;
		m_OnMouseLButtonUp(this, e);

		RadioButtonIsCheckedChangedEvent e(m_isChecked);
		m_OnIsCheckedChanged(this, e);

		e.Handled(this);
	}
}

bool RadioButton::MouseIsOver(float x, float y) noexcept
{
	EG_CORE_ASSERT(m_outerCircle != nullptr, "No outer circle");

	BOOL b;
	m_outerCircle->FillContainsPoint(D2D1::Point2F(x, y), D2D1::Matrix3x2F::Identity(), &b);
	return static_cast<bool>(b);
}


}