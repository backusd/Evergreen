#include "pch.h"
#include "Button.h"

namespace Evergreen
{
Button::Button(std::shared_ptr<DeviceResources> deviceResources, 
				std::unique_ptr<ColorBrush> backgroundBrush,
				std::unique_ptr<ColorBrush> borderBrush,
				float borderWidth,
				const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, margin),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_borderWidth(borderWidth),
	m_backgroundRect({ 0.0f, 0.0f, 1000.0f, 1000.0f }) // dummy values that will be written over when allowed region is updated
{
	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	// Move the background brush into the Button's layout. The button won't try to draw the background - will just allow the button's layout to draw the background
	// Create it with a dummy size - will get adjusted later when the allowed region is updated
	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		0.0f, 0.0f, 1000.0f, 1000.0f,
		std::move(m_backgroundBrush),
		"button layout");

	m_backgroundBrush = nullptr;
}

void Button::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush == nullptr, "Background brush is supposed to be nullptr");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	// Have the layout draw the background and contents of the brush
	m_layout->Render();

	// Draw the border last so it appears on top
	if (m_borderWidth > 0.0f)
		m_deviceResources->D2DDeviceContext()->DrawRectangle(m_backgroundRect, m_borderBrush->Get(), m_borderWidth);	
}

void Button::ButtonChanged() noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// Update the button rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	m_layout->Resize(m_backgroundRect);
}

void Button::OnMarginChanged() noexcept
{
	ButtonChanged();
}
void Button::OnAllowedRegionChanged() noexcept
{
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	// If using a non-SolidColorBrush, we need to update the draw region for the brushes
	m_borderBrush->SetDrawRegion(m_allowedRegion);

	ButtonChanged();
}

 // ================================================================================
/*
RoundedButton::RoundedButton(std::shared_ptr<DeviceResources> deviceResources) noexcept :
	Button(deviceResources)
{
}
void RoundedButton::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	auto context = m_deviceResources->D2DDeviceContext();

	context->FillRoundedRectangle(m_roundedRect, m_backgroundBrush->Get());

	if (m_borderWidth > 0.0f)
		context->DrawRoundedRectangle(m_roundedRect, m_borderBrush->Get(), m_borderWidth);

	m_layout->Render();
}
*/


}