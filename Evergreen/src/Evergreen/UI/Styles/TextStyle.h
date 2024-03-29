#pragma once
#include "pch.h"
#include "Style.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/UI/Utils/FontFamily.h"
#include "Evergreen/Rendering/DeviceResources.h"


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
	TextStyle(
		std::shared_ptr<DeviceResources> deviceResources,
		const std::string& name = "",
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
	);

	TextStyle(const TextStyle&);
	TextStyle& operator=(const TextStyle&);
	~TextStyle() override {}

	ND std::unique_ptr<Style> Duplicate() const override;

	ND Microsoft::WRL::ComPtr<IDWriteTextLayout4> CreateTextLayout(std::wstring text, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX);

	inline void SetOnTextFormatChanged(std::function<void()> func) noexcept { m_OnTextFormatChanged = func; }

	void FontFamily(const Evergreen::FontFamily& fontFamily);
	void FontWeight(DWRITE_FONT_WEIGHT weight);
	void FontStyle(DWRITE_FONT_STYLE style);
	void FontStretch(DWRITE_FONT_STRETCH stretch);
	void FontSize(float size);
	void Locale(const std::wstring& locale);
	void TextAlignment(DWRITE_TEXT_ALIGNMENT alignment);
	void ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment);
	void WordWrapping(DWRITE_WORD_WRAPPING wrapping);
	void Trimming(DWRITE_TRIMMING trimming);
	void TrimmingGranularity(DWRITE_TRIMMING_GRANULARITY granularity);

	ND inline Evergreen::FontFamily FontFamily() const noexcept { return m_fontFamily; }
	ND inline DWRITE_FONT_WEIGHT FontWeight() const noexcept { return m_fontWeight; }
	ND inline DWRITE_FONT_STYLE FontStyle() const noexcept { return m_fontStyle; }
	ND inline DWRITE_FONT_STRETCH FontStretch() const noexcept { return m_fontStretch; }
	ND inline float FontSize() const noexcept { return m_fontSize; }
	ND inline std::wstring Locale() const noexcept { return m_locale; }
	ND inline DWRITE_TEXT_ALIGNMENT TextAlignment() const noexcept { return m_textAlignment; }
	ND inline DWRITE_PARAGRAPH_ALIGNMENT ParagraphAlignment() const noexcept { return m_paragraphAlignment; }
	ND inline DWRITE_WORD_WRAPPING WordWrapping() const noexcept { return m_wordWrapping; }
	ND inline DWRITE_TRIMMING Trimming() const noexcept { return m_trimming; }

private:
	void Initialize();
	void UpdateTextFormat();

	std::function<void()> m_OnTextFormatChanged = []() {};

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
