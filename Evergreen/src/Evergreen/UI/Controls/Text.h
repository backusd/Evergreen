#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Styles/TextStyle.h"
#include "Evergreen/UI/Brushes/Brushes.h"


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
		const std::wstring& text = L"",
		std::unique_ptr<ColorBrush> brush = nullptr,
		std::unique_ptr<TextStyle> style = nullptr, 
		const Evergreen::Margin& margin = { 0 }) noexcept;
	Text(const Text& text) noexcept;		
	void operator=(const Text&) noexcept;
	virtual ~Text() noexcept override {}

	// Inherited from Control
	void Update() const noexcept override {}
	void Render() const noexcept override;
	void OnMouseMove(MouseMoveEvent& e) noexcept override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;

	// Text specific
	const std::wstring& GetText() const noexcept { return m_text; }
	TextStyle* GetTextStyle() const noexcept { return m_style.get(); }
	ColorBrush* GetColorBrush() const noexcept { return m_colorBrush.get(); }

	void SetText(const std::wstring& text) noexcept { m_text = text; TextChanged(); }
	void SetTextStyle(std::unique_ptr<TextStyle> style) noexcept;
	void SetColorBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_colorBrush = std::move(brush); }

private:
	void TextChanged();

	// React to changes made when base-class methods are called
	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;
	//void OnPositionChanged() noexcept override {}

	void UpdateBrushDrawRegion() noexcept;

	std::wstring				m_text;
	std::unique_ptr<TextStyle>	m_style;
	std::unique_ptr<ColorBrush>	m_colorBrush;
	DWRITE_TEXT_METRICS			m_textMetrics;

	Microsoft::WRL::ComPtr<IDWriteTextLayout4>	m_textLayout;
};
#pragma warning( pop )






}
