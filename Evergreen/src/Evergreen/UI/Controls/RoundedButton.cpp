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
								const Evergreen::Margin& margin) noexcept :
	Button(deviceResources, nullptr, std::move(borderBrush), borderWidth, margin),
	m_radiusX(radiusX),
	m_radiusY(radiusY)
{
	if (backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));
	else
		m_backgroundBrush = std::move(backgroundBrush);

	m_roundedRect = D2D1::RoundedRect(m_backgroundRect, m_radiusX, m_radiusY);

	std::unique_ptr<Evergreen::SolidColorBrush> transparentBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	m_layout->Brush(std::move(transparentBrush));
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

void RoundedButton::ButtonChanged() noexcept
{
	// Update the button rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	m_roundedRect = D2D1::RoundedRect(m_backgroundRect, m_radiusX, m_radiusY);

	m_layout->Resize(m_backgroundRect);
}

}