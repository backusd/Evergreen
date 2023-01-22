#include "pch.h"
#include "Text.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
Text::Text(std::shared_ptr<DeviceResources> deviceResources, 
			UI* ui,
			const D2D1_RECT_F& allowedRegion,
			const std::wstring& text, 
			std::unique_ptr<ColorBrush> brush, 
			std::unique_ptr<TextStyle> style, 
			const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, ui, allowedRegion, margin),
	m_text(text),
	m_style(std::move(style)),
	m_textLayout(nullptr),
	m_colorBrush(std::move(brush))
{
	// We cannot instantiate a SolidColorBrush as a default parameter, so the default is nullptr.
	// So if the colorBrush is nullptr, create a default SolidColorBrush
	if (m_colorBrush == nullptr)
		m_colorBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_style == nullptr)
		m_style = std::make_unique<TextStyle>(m_deviceResources);

	m_style->SetOnTextFormatChanged([this]() { TextChanged(); });

	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Call text changed to initialize the text layout
	TextChanged();
}

void Text::SetTextStyle(std::unique_ptr<TextStyle> style) noexcept 
{ 
	m_style = std::move(style); 
	m_style->SetOnTextFormatChanged([this]() { TextChanged(); });

	TextChanged(); 
}

void Text::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_textLayout != nullptr, "TextLayout is nullptr");
	EG_CORE_ASSERT(m_colorBrush != nullptr, "ColorBrush is nullptr");

	// NOTE: No need for any sort of error checking on the D2D API function calls.
	//       They don't report success/failure - you need to check the result of EndDraw() which we already do

	ID2D1DeviceContext6* context = m_deviceResources->D2DDeviceContext();
	context->PushAxisAlignedClip(m_allowedRegion, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// Don't set a transform - it will also affect the location of the AxisAlignedClip
	//context->SetTransform(D2D1::Matrix3x2F::Translation(m_topLeftPosition.x, m_topLeftPosition.y - 15.0f));

	context->DrawTextLayout(
		D2D1::Point2F(m_allowedRegion.left + m_margin.Left, m_allowedRegion.top + m_margin.Top),
		m_textLayout.Get(),
		m_colorBrush->Get(),
		D2D1_DRAW_TEXT_OPTIONS_CLIP			// <-- TODO: investigate these options, clipping is interesting
	);

	context->PopAxisAlignedClip();
}

void Text::TextChanged() noexcept
{
	EG_CORE_ASSERT(m_style != nullptr, "Style not created");
	EG_CORE_ASSERT(m_colorBrush != nullptr, "ColorBrush is nullptr");

	m_textLayout = m_style->CreateTextLayout(
		m_text, 
		(m_allowedRegion.right - m_margin.Right) - (m_allowedRegion.left + m_margin.Left), 
		(m_allowedRegion.bottom - m_margin.Bottom) - (m_allowedRegion.top + m_margin.Top)
	);
	m_textLayout->GetMetrics(&m_textMetrics);

	// If using a non-SolidColorBrush, we need to update the draw region for the brush
	UpdateBrushDrawRegion();
}

float Text::RightSideOfCharacterAtIndex(unsigned int index) const noexcept
{
	EG_CORE_ASSERT(index < m_text.size(), "Index is too large");
	EG_CORE_ASSERT(m_style != nullptr, "No TextStyle");

	std::wstring substring = m_text.substr(0, index + 1);

	DWRITE_TEXT_METRICS metrics;
	ZeroMemory(&metrics, sizeof(DWRITE_TEXT_METRICS));

	ComPtr<IDWriteTextLayout4> layout;
	layout = m_style->CreateTextLayout(
		substring,
		(m_allowedRegion.right - m_margin.Right) - (m_allowedRegion.left + m_margin.Left),
		(m_allowedRegion.bottom - m_margin.Bottom) - (m_allowedRegion.top + m_margin.Top)
	);
	layout->GetMetrics(&metrics);
	return Left() + metrics.widthIncludingTrailingWhitespace;
}



void Text::OnMarginChanged()
{
	// Just call TextChanged to completely recreate the TextLayout
	TextChanged();
}
void Text::OnAllowedRegionChanged()
{
	// Just call TextChanged to completely recreate the TextLayout
	TextChanged();
}

void Text::UpdateBrushDrawRegion() noexcept
{
	// If using a non-SolidColorBrush, we need to update the draw region for the brush
	m_colorBrush->SetDrawRegion(m_allowedRegion);
}

void Text::AddChar(char c, unsigned int index) noexcept
{
	EG_CORE_ASSERT(index <= m_text.size(), "Insert index is too large");

	if (index == m_text.size())
		m_text.push_back(c);
	else
		m_text.insert(index, 1, c);

	TextChanged();
}
void Text::RemoveChar(unsigned int index) noexcept
{
	EG_CORE_ASSERT(m_text.size() > 0, "No characters to remove");
	EG_CORE_ASSERT(index < m_text.size(), "Index is too large");

	m_text.erase(index, 1);
	TextChanged();
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

} // namespace Evergreen
