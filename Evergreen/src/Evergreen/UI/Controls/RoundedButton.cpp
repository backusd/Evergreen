#include "pch.h"
#include "RoundedButton.h"




namespace Evergreen
{
RoundedButton::RoundedButton(std::shared_ptr<DeviceResources> deviceResources,
								std::unique_ptr<ColorBrush> backgroundBrush,
								std::unique_ptr<ColorBrush> borderBrush,
								float radiusX,
								float radiusY,
								float borderWidth,
								const Evergreen::Margin& margin) :
	Button(deviceResources, nullptr, std::move(borderBrush), borderWidth, margin),
	m_radiusX(radiusX),
	m_radiusY(radiusY)
{
	// Background brush
	if (backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));
	else
		m_backgroundBrush = std::move(backgroundBrush);

	// Create rounded rect
	m_roundedRect = D2D1::RoundedRect(m_backgroundRect, m_radiusX, m_radiusY);

	// Make sure layout uses a transparent brush
	std::unique_ptr<Evergreen::SolidColorBrush> transparentBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	m_layout->Brush(std::move(transparentBrush));

	// Create the rounded rect geometry (used for determining if mouse is over button)
	GFX_THROW_INFO(
		m_deviceResources->D2DFactory()->CreateRoundedRectangleGeometry(m_roundedRect, m_roundedRectGeometry.ReleaseAndGetAddressOf())
	)
}

void RoundedButton::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	auto context = m_deviceResources->D2DDeviceContext();

	context->FillRoundedRectangle(m_roundedRect, m_backgroundBrush->Get());

	m_layout->Render();

	// Draw the border last so it appears on top
	if (m_borderWidth > 0.0f)
		context->DrawRoundedRectangle(m_roundedRect, m_borderBrush->Get(), m_borderWidth);





}

bool RoundedButton::ContainsPoint(float x, float y) const noexcept
{
	EG_CORE_ASSERT(m_roundedRectGeometry != nullptr, "No rounded rect geometry");

	BOOL b;
	m_roundedRectGeometry->FillContainsPoint(D2D1::Point2F(x, y), D2D1::Matrix3x2F::Identity(), &b);
	return static_cast<bool>(b);
}

void RoundedButton::ButtonChanged()
{
	// Update the button rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	m_roundedRect = D2D1::RoundedRect(m_backgroundRect, m_radiusX, m_radiusY);

	// Create the rounded rect geometry (used for determining if mouse is over button)
	GFX_THROW_INFO(
		m_deviceResources->D2DFactory()->CreateRoundedRectangleGeometry(m_roundedRect, m_roundedRectGeometry.ReleaseAndGetAddressOf())
	)

	m_layout->Resize(m_backgroundRect);
}
void RoundedButton::OnAllowedRegionChanged() noexcept
{
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	// If using a non-SolidColorBrush, we need to update the draw region for the brushes
	m_backgroundBrush->SetDrawRegion(m_allowedRegion);
	m_borderBrush->SetDrawRegion(m_allowedRegion);

	ButtonChanged();
}

}