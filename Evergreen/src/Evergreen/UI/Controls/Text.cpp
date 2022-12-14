#include "pch.h"
#include "Text.h"



namespace Evergreen
{
Text::Text(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& text, 
	std::unique_ptr<ColorBrush> brush, std::shared_ptr<TextStyle> style) noexcept :
	Control(deviceResources),
	m_text(text),
	m_style(style),
	m_textLayout(nullptr),
	m_colorBrush(std::move(brush))
{
	// We cannot instantiate a SolidColorBrush as a default parameter, so the default is nullptr.
	// So if the colorBrush is nullptr, create a default SolidColorBrush
	if (m_colorBrush == nullptr)
		m_colorBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

	if (m_style == nullptr)
		m_style = std::make_shared<TextStyle>(m_deviceResources);

	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Call text changed to initialize the text layout
	TextChanged();
}


void Text::Render() const noexcept
{
	EG_CORE_ASSERT(m_textLayout != nullptr, "TextLayout is nullptr");
	EG_CORE_ASSERT(m_colorBrush != nullptr, "ColorBrush is nullptr");

	ID2D1DeviceContext6* context = m_deviceResources->D2DDeviceContext();
	context->PushAxisAlignedClip(m_allowedRegion, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// Don't set a transform - it will also affect the location of the AxisAlignedClip
	//context->SetTransform(D2D1::Matrix3x2F::Translation(m_topLeftPosition.x, m_topLeftPosition.y - 15.0f));

	context->DrawTextLayout(
		m_topLeftPosition,
		m_textLayout.Get(),
		m_colorBrush->Get(),
		D2D1_DRAW_TEXT_OPTIONS_CLIP			// <-- TODO: investigate these options, clipping is interesting
	);

	context->PopAxisAlignedClip();
}

void Text::OnMouseMove(MouseMoveEvent& e) noexcept 
{

}
void Text::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept 
{

}
void Text::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept 
{

}

void Text::TextChanged()
{
	EG_CORE_ASSERT(m_style != nullptr, "Style not created");
	EG_CORE_ASSERT(m_colorBrush != nullptr, "ColorBrush is nullptr");

	m_textLayout = m_style->CreateTextLayout(m_text, m_allowedRegion.right - m_allowedRegion.left, m_allowedRegion.bottom - m_allowedRegion.top);
	m_textLayout->GetMetrics(&m_textMetrics);

	// If using a non-SolidColorBrush, we need to update the draw region for the brush
	m_colorBrush->SetDrawRegion(
		D2D1::RectF(
			m_textMetrics.left,
			m_textMetrics.top,
			m_textMetrics.left + m_textMetrics.width,
			m_textMetrics.top + m_textMetrics.height
		)
	);
}

void Text::AllowedRegion(D2D1_RECT_F region) noexcept
{
	m_allowedRegion = region;
	TextChanged();
}
void Text::AllowedRegion(float left, float top, float right, float bottom) noexcept
{
	m_allowedRegion.left = left;
	m_allowedRegion.top = top;
	m_allowedRegion.right = right;
	m_allowedRegion.bottom = bottom;
	TextChanged();
}

} // namespace Evergreen
