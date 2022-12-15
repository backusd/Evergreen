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
		std::shared_ptr<TextStyle> style = nullptr) noexcept;

	// I'm deleting these for now because in order to make a copy of m_colorBrush,
	// you would have to check if the incoming m_colorBrush can be cast to each
	// of the derived ColorBrush types and then create the appropriate unique_ptr.
	// I just don't want to do this at the moment...
	Text(const Text& text) noexcept = delete;		
	void operator=(const Text&) noexcept = delete;
	virtual ~Text() noexcept {}

	// Inherited from Control
	void Render() const noexcept override;
	void OnMouseMove(MouseMoveEvent& e) noexcept override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;
	void AllowedRegion(D2D1_RECT_F region) noexcept override;
	void AllowedRegion(float left, float top, float right, float bottom) noexcept override;

	// Text specific
	const std::wstring& GetText() const noexcept { return m_text; }
	std::shared_ptr<TextStyle> Style() const noexcept { return m_style; }


	void SetText(const std::wstring& text) noexcept { m_text = text; TextChanged(); }
	void Style(std::shared_ptr<TextStyle> style) noexcept { m_style = style; TextChanged(); }


public:
	void TextChanged();
private:

	std::wstring							m_text;
	std::shared_ptr<TextStyle>				m_style;
	std::unique_ptr<Evergreen::ColorBrush>	m_colorBrush;
	DWRITE_TEXT_METRICS						m_textMetrics;

	Microsoft::WRL::ComPtr<IDWriteTextLayout4>	m_textLayout;
};
#pragma warning( pop )






}
