#include "pch.h"
#include "TextStyle.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen 
{
	/*
	TextStyle::TextStyle(
		std::shared_ptr<DeviceResources> deviceResources, const std::string& name, Evergreen::Color color, Evergreen::FontFamily fontFamily,
		float fontSize, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch,
		DWRITE_TEXT_ALIGNMENT textAlignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment, DWRITE_WORD_WRAPPING wordWrapping, 
		DWRITE_TRIMMING trimming, const std::wstring& locale) noexcept :
			Style(deviceResources, name),
			m_color(color), m_fontFamily(fontFamily),
			m_fontWeight(fontWeight), m_fontStyle(fontStyle), m_fontStretch(fontStretch), m_fontSize(fontSize),
			m_locale(locale), m_textAlignment(textAlignment), m_paragraphAlignment(paragraphAlignment),
			m_wordWrapping(wordWrapping), m_trimming(trimming)
	{
		Initialize();
	}
	*/
	/*
	TextStyle::TextStyle(
		std::shared_ptr<DeviceResources> deviceResources,
		const std::string& name,
		const D2D1_COLOR_F& solidColor,
		Evergreen::FontFamily fontFamily,
		float fontSize,
		DWRITE_FONT_WEIGHT fontWeight,
		DWRITE_FONT_STYLE fontStyle,
		DWRITE_FONT_STRETCH fontStretch,
		DWRITE_TEXT_ALIGNMENT textAlignment,
		DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
		DWRITE_WORD_WRAPPING wordWrapping,
		DWRITE_TRIMMING trimming,
		const std::wstring& locale
	) noexcept :
		Style(deviceResources, name),
		m_colorBrush(std::make_unique<SolidColorBrush>(deviceResources, solidColor)), // In this constructor where a single color was specified, just create a SolidColorBrush
		m_fontFamily(fontFamily), m_fontWeight(fontWeight), m_fontStyle(fontStyle), m_fontStretch(fontStretch), 
		m_fontSize(fontSize), m_locale(locale), m_textAlignment(textAlignment), 
		m_paragraphAlignment(paragraphAlignment), m_wordWrapping(wordWrapping), m_trimming(trimming)
	{
		Initialize();
	}
	*/
	TextStyle::TextStyle(
		std::shared_ptr<DeviceResources> deviceResources,
		const std::string& name,
		std::unique_ptr<Evergreen::ColorBrush> colorBrush,
		Evergreen::FontFamily fontFamily,
		float fontSize,
		DWRITE_FONT_WEIGHT fontWeight,
		DWRITE_FONT_STYLE fontStyle,
		DWRITE_FONT_STRETCH fontStretch,
		DWRITE_TEXT_ALIGNMENT textAlignment,
		DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment,
		DWRITE_WORD_WRAPPING wordWrapping,
		DWRITE_TRIMMING trimming,
		const std::wstring& locale
	) noexcept :
		Style(deviceResources, name), 
		m_colorBrush(std::move(colorBrush)), m_fontFamily(fontFamily), m_fontWeight(fontWeight), m_fontStyle(fontStyle), 
		m_fontStretch(fontStretch), m_fontSize(fontSize), m_locale(locale), m_textAlignment(textAlignment),
		m_paragraphAlignment(paragraphAlignment), m_wordWrapping(wordWrapping), m_trimming(trimming)
	{
		// We cannot instantiate a SolidColorBrush as a default parameter, so the default is nullptr.
		// So if the colorBrush is nullptr, create a default SolidColorBrush
		if (m_colorBrush == nullptr)
			m_colorBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

		Initialize();
	}


	void TextStyle::Initialize() noexcept
	{
		EG_CORE_ASSERT(m_deviceResources != nullptr, "Cannot initialize Text Style - DeviceResources is nullptr");

		// Create the trimming structure
		ZeroMemory(&m_trimming, sizeof(DWRITE_TRIMMING));
		m_trimming.granularity = DWRITE_TRIMMING_GRANULARITY::DWRITE_TRIMMING_GRANULARITY_NONE;

		// Create the text format
		UpdateTextFormat();
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
