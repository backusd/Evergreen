#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Styles/TextStyle.h"


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
		std::shared_ptr<TextStyle> style = nullptr,
		D2D1::Matrix3x2F position = D2D1::Matrix3x2F::Translation(0.0f, 0.0f)) noexcept;
	Text(const Text& text) noexcept;
	void operator=(const Text&) noexcept;
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


	void SetText(const std::wstring& text) noexcept { m_text = text; }


private:
	void TextChanged();

	std::wstring				m_text;
	std::shared_ptr<TextStyle>	m_style;
	DWRITE_TEXT_METRICS			m_textMetrics;

	Microsoft::WRL::ComPtr<IDWriteTextLayout4>	m_textLayout;
};
#pragma warning( pop )
}
