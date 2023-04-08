#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Styles/TextStyle.h"
#include "Evergreen/UI/Brushes.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Text : public Control
{	
public:
	Text(std::shared_ptr<DeviceResources> deviceResources, 
		UI* ui,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		const std::wstring& text = L"",
		std::unique_ptr<ColorBrush> brush = nullptr,
		std::unique_ptr<TextStyle> style = nullptr, 
		const Evergreen::Margin& margin = { 0 }) noexcept;
	Text(const Text& text) noexcept = delete;
	void operator=(const Text&) noexcept = delete;
	virtual ~Text() noexcept override {}

	// Inherited from Control
	void Render() const override;

	// Text specific
	ND inline const std::wstring& GetText() const noexcept { return m_text; }
	ND inline TextStyle* GetTextStyle() const noexcept { return m_style.get(); }
	ND inline ColorBrush* GetColorBrush() const noexcept { return m_colorBrush.get(); }
	ND inline const DWRITE_TEXT_METRICS& GetTextMetrics() const noexcept { return m_textMetrics; }

	ND inline float Top() const noexcept { return m_allowedRegion.top + m_margin.Top + m_textMetrics.top; }
	ND inline float Bottom() const noexcept { return Top() + m_textMetrics.height; }
	ND inline float Left() const noexcept { return m_allowedRegion.left + m_margin.Left; }
	ND inline float Right() const noexcept { return Left() + m_textMetrics.widthIncludingTrailingWhitespace; }
	ND float RightSideOfCharacterAtIndex(unsigned int index) const noexcept;
	ND inline float Width() const noexcept { return m_textMetrics.widthIncludingTrailingWhitespace; }
	ND inline float Height() const noexcept { return m_textMetrics.height; }
	ND unsigned int Size() const noexcept { return static_cast<unsigned int>(m_text.size()); }


	void SetText(const std::wstring& text) noexcept 
	{ 
		m_text = text; 
		TextChanged(); 
	}
	void SetTextStyle(std::unique_ptr<TextStyle> style) noexcept;
	void SetColorBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_colorBrush = std::move(brush); }

	void AddChar(char c, unsigned int index) noexcept;
	void RemoveChar(unsigned int index) noexcept;

	virtual ControlType GetControlType() const noexcept { return ControlType::Text; }

private:
	void TextChanged() noexcept;

	// React to changes made when base-class methods are called
	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;
	void UpdateBrushDrawRegion() noexcept;

	std::wstring				m_text;
	std::unique_ptr<TextStyle>	m_style;
	std::unique_ptr<ColorBrush>	m_colorBrush;
	DWRITE_TEXT_METRICS			m_textMetrics;

	Microsoft::WRL::ComPtr<IDWriteTextLayout4>	m_textLayout;
};
#pragma warning( pop )






}
