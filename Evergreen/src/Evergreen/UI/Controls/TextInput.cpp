#include "pch.h"
#include "TextInput.h"


namespace Evergreen
{
TextInput::TextInput(std::shared_ptr<DeviceResources> deviceResources,
						const D2D1_RECT_F& allowedRegion,
						const std::wstring& placeholderText,
						std::unique_ptr<ColorBrush> placeholderBrush,
						std::unique_ptr<TextStyle> placeholderStyle,
						std::unique_ptr<ColorBrush> inputTextBrush,
						std::unique_ptr<TextStyle> inputTextStyle,
						std::unique_ptr<ColorBrush> backgroundBrush,
						std::unique_ptr<ColorBrush> borderBrush,
						float borderWidth,
						const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, allowedRegion, margin),
	m_placeholderText(placeholderText),
	m_placeholderTextBrush(std::move(placeholderBrush)),
	m_placeholderTextStyle(std::move(placeholderStyle)),
	m_inputText(L""),
	m_inputTextBrush(std::move(inputTextBrush)),
	m_inputTextStyle(std::move(inputTextStyle)),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_borderWidth(borderWidth),
	m_backgroundRect({ 0.0f, 0.0f, 1000.0f, 1000.0f }) // dummy values that will be written over when allowed region is updated
{
	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Brushes
	if (m_placeholderTextBrush == nullptr)
		m_placeholderTextBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_inputTextBrush == nullptr)
		m_inputTextBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::WhiteSmoke));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));

	// Text Styles
	if (m_placeholderTextStyle == nullptr)
	{
		m_placeholderTextStyle = std::make_unique<TextStyle>(
			m_deviceResources,
			"TextInput Placeholder Style",
			Evergreen::FontFamily::Calibri,
			12.0f,
			DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
			DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER, // Vertically align to the center of the layout
			DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
		);
	}

	if (m_inputTextStyle == nullptr)
	{
		m_inputTextStyle = std::make_unique<TextStyle>(
			m_deviceResources,
			"TextInput Input Style",
			Evergreen::FontFamily::Calibri,
			12.0f,
			DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
			DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER, // Vertically align to the center of the layout
			DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
			);
	}

	// Create the layout with a dummy size - will be updated when TextInputChanged() is called
	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		0.0f, 0.0f, 1000.0f, 1000.0f,
		nullptr, // Don't pass a brush to the layout - Button should draw the background, not the layout
		"TextInput Layout");

	// Add a single row and column to span the layout
	// NOTE: The user can optionally decide to add a second column by calling AddRightColumn() which will create
	//       a second column and a sublayout for Row:0 Column:1. They can then get that sublayout by calling GetRightColumnLayout()
	//       and can therefore put whatever they want in that layout
	m_layout->AddRow({ RowColumnType::STAR, 1.0f });
	m_layout->AddColumn({ RowColumnType::STAR, 1.0f });

	// Create the Text control that will be displayed in the TextInput layout
	m_text = m_layout->CreateControl<Text>(
		deviceResources,
		m_placeholderText,
		std::move(m_placeholderTextBrush->Duplicate()),
		std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(m_placeholderTextStyle->Duplicate().release())))
	);

	m_text->MarginLeft(4.0f);

	// Call TextInputChanged to update the layout
	TextInputChanged();

	// Set the initial text to be just the placeholder text
	SetTextToPlaceholder();
}

void TextInput::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	// Draw the background
	m_deviceResources->D2DDeviceContext()->FillRectangle(m_backgroundRect, m_backgroundBrush->Get());

	// Have the layout draw the background and text
	m_layout->Render();

	// Draw the border last so it appears on top
	if (m_borderWidth > 0.0f)
		m_deviceResources->D2DDeviceContext()->DrawRectangle(m_backgroundRect, m_borderBrush->Get(), m_borderWidth);
}

void TextInput::TextInputChanged() noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// Update the background rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	m_layout->Resize(m_backgroundRect);
}

void TextInput::OnMarginChanged()
{
	TextInputChanged();
}
void TextInput::OnAllowedRegionChanged()
{
	TextInputChanged();
}

void TextInput::SetTextToPlaceholder() noexcept
{
	EG_CORE_ASSERT(m_text != nullptr, "Text control should not be nullptr");
	EG_CORE_ASSERT(m_placeholderTextBrush != nullptr, "Placeholder brush should not be nullptr");
	EG_CORE_ASSERT(m_placeholderTextStyle != nullptr, "Placeholder text style should not be nullptr");

	m_text->SetText(m_placeholderText);
	m_text->SetColorBrush(std::move(m_placeholderTextBrush->Duplicate()));
	m_text->SetTextStyle(std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(m_placeholderTextStyle->Duplicate().release()))));
}
void TextInput::SetTextToInput() noexcept
{
	EG_CORE_ASSERT(m_text != nullptr, "Text control should not be nullptr");
	EG_CORE_ASSERT(m_inputTextBrush != nullptr, "Input brush should not be nullptr");
	EG_CORE_ASSERT(m_inputTextStyle != nullptr, "Input text style should not be nullptr");

	m_text->SetText(m_inputText);
	m_text->SetColorBrush(std::move(m_inputTextBrush->Duplicate()));
	m_text->SetTextStyle(std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(m_inputTextStyle->Duplicate().release()))));
}




}