#include "pch.h"
#include "TextStyle.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen 
{
	TextStyle::TextStyle(
		std::shared_ptr<DeviceResources> deviceResources, Evergreen::Color color, Evergreen::FontFamily fontFamily,
		DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize,
		const std::wstring& locale, DWRITE_TEXT_ALIGNMENT textAlignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
		DWRITE_WORD_WRAPPING wordWrapping, DWRITE_TRIMMING trimming) noexcept :
			m_deviceResources(deviceResources), m_color(color), m_fontFamily(fontFamily),
			m_fontWeight(fontWeight), m_fontStyle(fontStyle), m_fontStretch(fontStretch), m_fontSize(fontSize),
			m_locale(locale), m_textAlignment(textAlignment), m_paragraphAlignment(paragraphAlignment),
			m_wordWrapping(wordWrapping), m_trimming(trimming)
	{
		Initialize();
	}
	TextStyle::TextStyle(const TextStyle& rhs) noexcept :
		m_deviceResources(rhs.m_deviceResources), m_color(rhs.m_color), m_fontFamily(rhs.m_fontFamily),
		m_fontWeight(rhs.m_fontWeight), m_fontStyle(rhs.m_fontStyle), m_fontStretch(rhs.m_fontStretch), 
		m_fontSize(rhs.m_fontSize),	m_locale(rhs.m_locale), m_textAlignment(rhs.m_textAlignment), 
		m_paragraphAlignment(rhs.m_paragraphAlignment), m_wordWrapping(rhs.m_wordWrapping), m_trimming(rhs.m_trimming)
	{
		Initialize();
	}
	void TextStyle::operator=(const TextStyle& rhs) noexcept
	{
		m_deviceResources = rhs.m_deviceResources;
		m_color = rhs.m_color;
		m_fontFamily = rhs.m_fontFamily;
		m_fontWeight = rhs.m_fontWeight;
		m_fontStyle = rhs.m_fontStyle; 
		m_fontStretch = rhs.m_fontStretch;
		m_fontSize = rhs.m_fontSize;
		m_locale = rhs.m_locale;
		m_textAlignment = rhs.m_textAlignment;
		m_paragraphAlignment = rhs.m_paragraphAlignment;
		m_wordWrapping = rhs.m_wordWrapping;
		m_trimming = rhs.m_trimming;

		Initialize();
	}
	void TextStyle::Initialize() noexcept
	{
		EG_CORE_ASSERT(m_deviceResources != nullptr, "Cannot initialize Text Style - DeviceResources is nullptr");

		UpdateBrush();

		// Create the trimming structure
		ZeroMemory(&m_trimming, sizeof(DWRITE_TRIMMING));
		m_trimming.granularity = DWRITE_TRIMMING_GRANULARITY::DWRITE_TRIMMING_GRANULARITY_NONE;

		// Create the text format
		UpdateTextFormat();
	}

	void TextStyle::UpdateBrush() noexcept
	{
		m_deviceResources->D2DDeviceContext()->CreateSolidColorBrush(
			D2D1::ColorF(m_color.R(), m_color.G(), m_color.B(), m_color.A()), 
			m_colorBrush.ReleaseAndGetAddressOf()
		);
	}

	void TextStyle::UpdateTextFormat() noexcept
	{
		std::string ff = m_fontFamily.Get();
		std::wstring fontFamily(ff.begin(), ff.end());

		
		ComPtr<IDWriteTextFormat> textFormat = nullptr;
		GFX_THROW_INFO(
			m_deviceResources->DWriteFactory()->CreateTextFormat(
				fontFamily.c_str(),
				m_fontCollection.Get(),
				m_fontWeight,
				m_fontStyle,
				m_fontStretch,
				m_fontSize,
				m_locale.c_str(),
				textFormat.ReleaseAndGetAddressOf()
			)
		);

		GFX_THROW_INFO(textFormat.As(&m_textFormat));

		GFX_THROW_INFO(m_textFormat->SetTextAlignment(m_textAlignment));
		GFX_THROW_INFO(m_textFormat->SetParagraphAlignment(m_paragraphAlignment));
		GFX_THROW_INFO(m_textFormat->SetWordWrapping(m_wordWrapping));
		GFX_THROW_INFO(m_textFormat->SetTrimming(&m_trimming, nullptr));		
	}

	void TextStyle::Color(const Evergreen::Color& color) noexcept
	{
		m_color = color;
		UpdateBrush();
	}
	void TextStyle::FontFamily(const Evergreen::FontFamily& fontFamily) noexcept 
	{ 
		m_fontFamily = fontFamily;
		UpdateTextFormat();
	}
	void TextStyle::FontWeight(DWRITE_FONT_WEIGHT weight) noexcept 
	{ 
		m_fontWeight = weight; 
		UpdateTextFormat();
	}
	void TextStyle::FontStyle(DWRITE_FONT_STYLE style) noexcept 
	{ 
		m_fontStyle = style; 
		UpdateTextFormat();
	}
	void TextStyle::FontStretch(DWRITE_FONT_STRETCH stretch) noexcept 
	{ 
		m_fontStretch = stretch; 
		UpdateTextFormat();
	}
	void TextStyle::FontSize(float size) noexcept 
	{ 
		m_fontSize = size; 
		UpdateTextFormat();
	}
	void TextStyle::Locale(const std::wstring& locale) noexcept 
	{ 
		m_locale = locale; 
		UpdateTextFormat();
	}
	void TextStyle::TextAlignment(DWRITE_TEXT_ALIGNMENT alignment) noexcept 
	{ 
		m_textAlignment = alignment; 
		UpdateTextFormat();
	}
	void TextStyle::ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment) noexcept 
	{ 
		m_paragraphAlignment = alignment; 
		UpdateTextFormat();
	}
	void TextStyle::WordWrapping(DWRITE_WORD_WRAPPING wrapping) noexcept 
	{ 
		m_wordWrapping = wrapping; 
		UpdateTextFormat();
	}
	void TextStyle::Trimming(DWRITE_TRIMMING trimming) noexcept 
	{ 
		m_trimming = trimming; 
		UpdateTextFormat();
	}
	void TextStyle::TrimmingGranularity(DWRITE_TRIMMING_GRANULARITY granularity) noexcept
	{
		m_trimming.granularity = granularity;
		UpdateTextFormat();
	}

	ComPtr<IDWriteTextLayout4> TextStyle::CreateTextLayout(std::wstring text, float maxWidth, float maxHeight) noexcept
	{
		
		ComPtr<IDWriteTextLayout> textLayout;
		ComPtr<IDWriteTextLayout4> textLayout4;

		GFX_THROW_INFO(
			m_deviceResources->DWriteFactory()->CreateTextLayout(
				text.c_str(),
				(uint32_t)text.length(),
				m_textFormat.Get(),
				maxWidth,
				maxHeight,
				textLayout.ReleaseAndGetAddressOf()
			)
		);
		
		GFX_THROW_INFO(textLayout.As(&textLayout4));

		return textLayout4;
	}
}
