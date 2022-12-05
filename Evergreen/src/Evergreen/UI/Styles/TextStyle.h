#pragma once
#include "pch.h"
#include "Style.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
//#include "Evergreen/UI/Color.h"
#include "FontFamily.h"
#include "Evergreen/Rendering/DeviceResources.h"
#include "Evergreen/UI/Colors/Brushes.h"



namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API TextStyle : public Style
{
public:
	/*
	TextStyle(
		std::shared_ptr<DeviceResources> deviceResources,
		const std::string& name = "",
		Evergreen::Color color = Evergreen::Color::Black,
		Evergreen::FontFamily fontFamily = Evergreen::FontFamily::Calibri,
		float fontSize = 12.0f,
		DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		DWRITE_WORD_WRAPPING wordWrapping = DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP,
		DWRITE_TRIMMING trimming = DWRITE_TRIMMING(),
		const std::wstring& locale = L"en-US"
	) noexcept;
	*/
	/*
	TextStyle(
		std::shared_ptr<DeviceResources> deviceResources,
		const std::string& name = "",
		const D2D1_COLOR_F& solidColor = D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
		Evergreen::FontFamily fontFamily = Evergreen::FontFamily::Calibri,
		float fontSize = 12.0f,
		DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		DWRITE_WORD_WRAPPING wordWrapping = DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP,
		DWRITE_TRIMMING trimming = DWRITE_TRIMMING(),
		const std::wstring& locale = L"en-US"
	) noexcept;
	*/
	TextStyle(
		std::shared_ptr<DeviceResources> deviceResources,
		const std::string& name = "",
		std::unique_ptr<Evergreen::ColorBrush> colorBrush = nullptr,
		Evergreen::FontFamily fontFamily = Evergreen::FontFamily::Calibri,
		float fontSize = 12.0f,
		DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		DWRITE_WORD_WRAPPING wordWrapping = DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP,
		DWRITE_TRIMMING trimming = DWRITE_TRIMMING(),
		const std::wstring& locale = L"en-US"
	) noexcept;

	// Deleting these because I'm not yet sure the best way to copy the unique_ptr to ColorBrush
	TextStyle(const TextStyle&) noexcept = delete;
	void operator=(const TextStyle&) noexcept = delete;

	Microsoft::WRL::ComPtr<IDWriteTextLayout4> CreateTextLayout(std::wstring text, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX) noexcept;

	void Color(const D2D1_COLOR_F& color) noexcept { m_colorBrush = std::make_unique<SolidColorBrush>(m_deviceResources, color); }
	void ColorBrush(std::unique_ptr<ColorBrush> colorBrush) noexcept { m_colorBrush = std::move(colorBrush); }

	void FontFamily(const Evergreen::FontFamily& fontFamily) noexcept;
	void FontWeight(DWRITE_FONT_WEIGHT weight) noexcept;
	void FontStyle(DWRITE_FONT_STYLE style) noexcept;
	void FontStretch(DWRITE_FONT_STRETCH stretch) noexcept;
	void FontSize(float size) noexcept;
	void Locale(const std::wstring& locale) noexcept;
	void TextAlignment(DWRITE_TEXT_ALIGNMENT alignment) noexcept;
	void ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment) noexcept;
	void WordWrapping(DWRITE_WORD_WRAPPING wrapping) noexcept;
	void Trimming(DWRITE_TRIMMING trimming) noexcept;
	void TrimmingGranularity(DWRITE_TRIMMING_GRANULARITY granularity) noexcept;

	ID2D1Brush* ColorBrush() const noexcept { return m_colorBrush->Get(); }	
	Evergreen::FontFamily FontFamily() const noexcept { return m_fontFamily; }
	DWRITE_FONT_WEIGHT FontWeight() const noexcept { return m_fontWeight; }
	DWRITE_FONT_STYLE FontStyle() const noexcept { return m_fontStyle; }
	DWRITE_FONT_STRETCH FontStretch() const noexcept { return m_fontStretch; }
	float FontSize() const noexcept { return m_fontSize; }
	std::wstring Locale() const noexcept { return m_locale; }
	DWRITE_TEXT_ALIGNMENT TextAlignment() const noexcept { return m_textAlignment; }
	DWRITE_PARAGRAPH_ALIGNMENT ParagraphAlignment() const noexcept { return m_paragraphAlignment; }
	DWRITE_WORD_WRAPPING WordWrapping() const noexcept { return m_wordWrapping; }
	DWRITE_TRIMMING Trimming() const noexcept { return m_trimming; }

private:
	void Initialize() noexcept;
	void UpdateTextFormat() noexcept;

	std::unique_ptr<Evergreen::ColorBrush>			m_colorBrush;

	Evergreen::FontFamily							m_fontFamily;
	Microsoft::WRL::ComPtr<IDWriteFontCollection>	m_fontCollection; // Not sure what to do with this. Okay to always be nullptr
	DWRITE_FONT_WEIGHT								m_fontWeight;
	DWRITE_FONT_STYLE								m_fontStyle;
	DWRITE_FONT_STRETCH								m_fontStretch;
	float											m_fontSize;
	std::wstring									m_locale;
	DWRITE_TEXT_ALIGNMENT							m_textAlignment;
	DWRITE_PARAGRAPH_ALIGNMENT						m_paragraphAlignment;
	DWRITE_WORD_WRAPPING							m_wordWrapping;
	DWRITE_TRIMMING									m_trimming;

	Microsoft::WRL::ComPtr<IDWriteTextFormat3>		m_textFormat;
};
#pragma warning ( pop )
}
