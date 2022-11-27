#include "pch.h"
#include "Text.h"



namespace Evergreen
{
Text::Text(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& text, std::shared_ptr<TextStyle> style) noexcept : 
	Control(deviceResources),
	m_text(text),
	m_style(style),
	m_textLayout(nullptr)
{
	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Call text changed to initialize the text layout
	TextChanged();
}
Text::Text(const Text& text) noexcept :
	Control(text.m_deviceResources),
	m_text(text.m_text),
	m_style(text.m_style),
	m_textLayout(nullptr)
{
	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Call text changed to initialize the text layout
	TextChanged();
}
void Text::operator=(const Text& rhs) noexcept
{
	m_deviceResources = rhs.m_deviceResources;
	m_text = rhs.m_text;
	m_style = rhs.m_style;
	m_textLayout = nullptr;

	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Call text changed to initialize the text layout
	TextChanged();
}

void Text::Render() const noexcept
{
	ID2D1DeviceContext6* context = m_deviceResources->D2DDeviceContext();
	context->PushAxisAlignedClip(m_allowedRegion, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// Don't set a transform - it will also affect the location of the AxisAlignedClip
	//context->SetTransform(D2D1::Matrix3x2F::Translation(m_topLeftPosition.x, m_topLeftPosition.y - 15.0f));

	context->DrawTextLayout(
		m_topLeftPosition,
		m_textLayout.Get(),
		m_style->ColorBrush(),
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
	/*
	D2D1_RECT_F rect = GetParentRect();

	// Adjust the rect with the margins
	rect = D2D1::RectF(rect.left + m_marginLeft, rect.top + m_marginTop, rect.right - m_marginRight, rect.bottom - m_marginBottom);

	float maxWidth = rect.right - rect.left; // std::max(0, width of parent rect)
	float maxHeight = rect.bottom - rect.top; // std::max(0, height of parent rect)

	m_textLayout = m_textTheme->CreateTextLayout(m_text, maxWidth, maxHeight);

	ThrowIfFailed(m_textLayout->GetMetrics(&m_textMetrics));


	// Update Screen Translation ---
	m_screenTranslation = D2D1::Matrix3x2F::Translation(rect.left, rect.top);
	*/

	m_textLayout = m_style->CreateTextLayout(m_text, m_allowedRegion.right - m_allowedRegion.left, m_allowedRegion.bottom - m_allowedRegion.top);
	m_textLayout->GetMetrics(&m_textMetrics);
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
